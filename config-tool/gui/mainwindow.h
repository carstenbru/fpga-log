#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <map>
#include <QModelIndex>
#include "cobject.h"
#include "datastreamview.h"
#include "datatype.h"
#include "dataloggerothermodel.h"
#include "outputgenerator.h"

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
    void newOutputGenerator();

    Ui::MainWindow *ui;

    DatastreamView* datastreamView;

    DataLogger dataLogger;
    DataLoggerOtherModel otherModel;

    OutputGenerator* outputGenerator;

    bool dataLoggerSaved;
    bool bitfileGenerated;
public slots:
    void newObject();
    void showConfigDialog(CObject& object);
    void targetConfig();
    void generate();
    void synthesize();
    void flash();
private slots:
    void otherObjectConfig(QModelIndex index);
    void outputGeneratorFinished(bool errorOccured);
    void dataLoggerChanged();
};

#endif // MAINWINDOW_H
