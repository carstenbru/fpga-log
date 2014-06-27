#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <map>
#include <QModelIndex>
#include "cobject.h"
#include "datastreamview.h"
#include "datatype.h"
#include "dataloggerothermodel.h"

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
    DataLoggerOtherModel otherModel;
public slots:
    void newObject();
    void showConfigDialog(CObject& object);
    void targetConfig();
private slots:
    void otherObjectConfig(QModelIndex index);
};

#endif // MAINWINDOW_H
