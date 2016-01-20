#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <map>
#include <QModelIndex>
#include <string>
#include "cobject.h"
#include "datastreamview.h"
#include "datatype.h"
#include "dataloggerothermodel.h"
#include "outputgenerator.h"
#include "configobjectdialog.h"

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
    bool newOutputGenerator();
    void refreshWindowTitle();
    bool checkAndAskSave();
    void setDataLoggerPath(std::string newPath);
    void reparseLocalHeaders(std::string path);

    Ui::MainWindow *ui;

    DatastreamView* datastreamView;

    DataLogger* dataLogger;
    std::string dataLoggerPath;
    bool dataLoggerSaved;
    bool bitfileGenerated;

    OutputGenerator* outputGenerator;
    DataLoggerOtherModel otherModel;
public slots:
    void closeEvent(QCloseEvent* event);

    void newObject();
    void newObject(DataTypeStruct *type, ConfigObjectDialog *confDialog);
    void showConfigDialog(CObject& object);
    void targetConfig();
    void generate();
    void synthesize();
    void flash();

    void newLogger();
    void open();
    void save();
    void saveAs();

    void pinOverview();
private slots:
    void otherObjectConfig(QModelIndex index);
    void outputGeneratorFinished(bool errorOccured);
    void dataLoggerChanged();
};

#endif // MAINWINDOW_H
