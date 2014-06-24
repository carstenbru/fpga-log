#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <map>
#include "datastreamview.h"
#include "datatype.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    virtual void show();
private:
    Ui::MainWindow *ui;

    DatastreamView* datastreamView;

    DataLogger dataLogger;
public slots:
    void newObject();
};

#endif // MAINWINDOW_H
