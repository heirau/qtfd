#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSignalMapper>
#include <QPushButton>
#include <QLineEdit>
#include <QTreeWidget>
#include "files.h"
#include "xmlread.h"
#include "search.h"
#include "flowlayout.h"
#include "categories.h"
#include "timeline.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void addButtons(QList <int> indexes);
    QList<QPushButton *> buttonlist;
    XMLRead *xmlRead;
    Search *search;
    QList <int> years;
public slots:
    bool openfile(const QString filename);
private:
    void timeLine();
    void createTags();
    QSignalMapper* signalMapper;
    Ui::MainWindow *ui;
    Timeline *widget;
    QTreeWidget *searchWidget; // Sisaltaa vasemman reunan kategorianapit
    int window_width;
    int window_height;
    QLineEdit *searchbar;
    FlowLayout *flowLayout;
    void addCategoryButtons();
    Categories *cats;
private slots:
    void selectCategory(QTreeWidgetItem* item,int n);
};

#endif // MAINWINDOW_H
