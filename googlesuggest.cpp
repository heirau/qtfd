/****************************************************************************
 **
 ** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** You may use this file under the terms of the BSD license as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
 **     the names of its contributors may be used to endorse or promote
 **     products derived from this software without specific prior written
 **     permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

 #include "googlesuggest.h"

 GSuggestCompletion::GSuggestCompletion(Search *s, QLineEdit *parent): QObject(parent), editor(parent)
 {
     popup = new QTreeWidget;
     popup->setWindowFlags(Qt::Popup);
     popup->setFocusPolicy(Qt::NoFocus);
     popup->setFocusProxy(parent);
     popup->setMouseTracking(true);

     popup->setColumnCount(2);
     popup->setUniformRowHeights(true);
     popup->setRootIsDecorated(false);
     popup->setEditTriggers(QTreeWidget::NoEditTriggers);
     popup->setSelectionBehavior(QTreeWidget::SelectRows);
     popup->setFrameStyle(QFrame::Box | QFrame::Plain);
     popup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
     popup->header()->hide();

     popup->installEventFilter(this);

     this->search = s;

     connect(popup, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
             SLOT(doneCompletion()));

     timer = new QTimer(this);
     timer->setSingleShot(true);
     timer->setInterval(500);
     connect(timer, SIGNAL(timeout()), SLOT(autoSuggest()));
     connect(editor, SIGNAL(textEdited(QString)), timer, SLOT(start()));
 }

 GSuggestCompletion::~GSuggestCompletion()
 {
     delete popup;
 }

 bool GSuggestCompletion::eventFilter(QObject *obj, QEvent *ev)
 {
     if (obj != popup)
         return false;

     if (ev->type() == QEvent::MouseButtonPress) {
         popup->hide();
         editor->setFocus();
         return true;
     }

     if (ev->type() == QEvent::KeyPress) {

         bool consumed = false;
         int key = static_cast<QKeyEvent*>(ev)->key();
         switch (key) {
         case Qt::Key_Enter:
         case Qt::Key_Return:
             doneCompletion();
             consumed = true;

         case Qt::Key_Escape:
             editor->setFocus();
             popup->hide();
             consumed = true;

         case Qt::Key_Up:
         case Qt::Key_Down:
         case Qt::Key_Home:
         case Qt::Key_End:
         case Qt::Key_PageUp:
         case Qt::Key_PageDown:
             break;

         default:
             editor->setFocus();
             editor->event(ev);
             popup->hide();
             break;
         }

         return consumed;
     }

     return false;
 }

 void GSuggestCompletion::showCompletion(const QStringList &choices, const QStringList &hits)
 {

     if (choices.isEmpty() || choices.count() != hits.count())
         return;

     const QPalette &pal = editor->palette();
     QColor color = pal.color(QPalette::Disabled, QPalette::WindowText);

     popup->setUpdatesEnabled(false);
     popup->clear();
     for (int i = 0; i < choices.count(); ++i) {
         QTreeWidgetItem * item;
         item = new QTreeWidgetItem(popup);
         item->setText(0, choices[i]);
         item->setText(1, hits[i]);
         item->setTextAlignment(1, Qt::AlignRight);
         item->setTextColor(1, color);
     }
     popup->setCurrentItem(popup->topLevelItem(0));
     popup->resizeColumnToContents(0);
     popup->resizeColumnToContents(1);
     popup->adjustSize();
     popup->setUpdatesEnabled(true);

     int h = popup->sizeHintForRow(0) * qMin(7, choices.count()) + 3;
     popup->resize(popup->width(), h);

     popup->move(editor->mapToGlobal(QPoint(0, editor->height())));
     popup->setFocus();
     popup->show();
 }

 void GSuggestCompletion::doneCompletion()
 {
     timer->stop();
     popup->hide();
     editor->setFocus();
     QTreeWidgetItem *item = popup->currentItem();
     if (item) {
         editor->setText(item->text(0));
         QMetaObject::invokeMethod(editor, "returnPressed");
     }
 }

 void GSuggestCompletion::autoSuggest()
 {
     QString str = editor->text();
     QStringList choices;
     QStringList hits;

     // find words for the given string
     QList <TagRefs> tagrefs = this->search->find(str);
     foreach (TagRefs tr, tagrefs)
     {
         choices << tr.tagname;
         hits << QString::number(tr.filelist.count());
     }

     showCompletion(choices, hits);
 }

 void GSuggestCompletion::preventSuggest()
 {
     timer->stop();
 }

