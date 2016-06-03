#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <map>
#include <QModelIndex>
#include <string>
#include <QMenu>
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
    void clearErrorList();

    Ui::MainWindow *ui;

    DatastreamView* datastreamView;

    DataLogger* dataLogger;
    std::string dataLoggerPath;
    bool dataLoggerSaved;
    bool bitfileGenerated;

    OutputGenerator* outputGenerator;
    DataLoggerOtherModel otherModel;

    std::string clipboardObjectDescription;

    QMenu* expertMenu;
public slots:
    void closeEvent(QCloseEvent* event);

    void newObject();
    void newObject(DataTypeStruct *type, ConfigObjectDialog *confDialog);
    void pasteModule();
    void pasteModule(QPoint pos);
    void showConfigDialog(CObject& object);
    void targetConfig();
    void generate();
    void synthesize();
    void synthesizeOnly();
    void flash();

    void developerTools();
    void coreAssigner();

    void newLogger();
    void open();
    void save();
    void saveAs();

    void pinOverview();

    void copyObject(std::string objectDescription);

    void addErrorToList(std::string message);
private slots:
    void otherObjectConfig(QModelIndex index);
    void otherObjectMenu(QPoint pos);
    void otherObjectCopy();
    void outputGeneratorFinished(bool errorOccured, bool timingError);
    void dataLoggerChanged();
    void expertModeChanged(bool expertMode);
};

#endif // MAINWINDOW_H
