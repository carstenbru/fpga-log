/**
 * @file mainwindow.h
 * @brief MainWindow GUI class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

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

/**
 * @brief config-tool main window GUI class
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief constructor
     *
     * @param parent parent widget
     */
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /**
     * @brief inherited show method called when the window is shown
     */
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
    /**
     * @brief window close event slot
     *
     * @param event close event
     */
    void closeEvent(QCloseEvent* event);

    /**
     * @brief slot for requesting creation of a new object
     */
    void newObject();

    /**
     * @brief slot for requesting creation of a new object a a specific type
     *
     * @param type type of the new object
     * @param confDialog configuration dialog that is requesting the object generation
     */
    void newObject(DataTypeStruct *type, ConfigObjectDialog *confDialog);

    /**
     * @brief slot to notify about a moudle paste
     */
    void pasteModule();

    /**
     * @brief slot to notify about a moudle paste
     *
     * @param pos position of the paste event
     */
    void pasteModule(QPoint pos);

    /**
     * @brief slot to request a configuration dialog for an object
     *
     * @param object object the configuration dialog should be created for
     */
    void showConfigDialog(CObject& object);

    /**
     * @brief slot to request a target configuration dialog
     */
    void targetConfig();

    /**
     * @brief slot to request system files generation
     */
    void generate();

    /**
     * @brief slot to request system synthesis (with updating all system files before)
     */
    void synthesize();

    /**
     * @brief slot to request system synthesis (without updating all system files before)
     */
    void synthesizeOnly();

    /**
     * @brief slot to request flashing of the datalogger
     *
     * If the bitmap file is not up-to-date the user will be asked if the system should be generated first
     */
    void flash();

    /**
     * @brief slot to force stopping of the currently executed task
     */
    void stopTask();

    /**
     * @brief slot to request the developer tools dialog
     */
    void developerTools();

    /**
     * @brief slot to start the core assigner
     */
    void coreAssigner();

    /**
     * @brief slot to request the core assigner parameters dialog
     */
    void coreAssignerParameters();

    /**
     * @brief slot to request creation of a new logger
     */
    void newLogger();

    /**
     * @brief slot to open a logger
     */
    void open();

    /**
     * @brief slot to save a logger
     */
    void save();

    /**
     * @brief slot to save a logger in a different location
     */
    void saveAs();

    /**
     * @brief slot to request the pin overview dialog
     */
    void pinOverview();

    /**
     * @brief slot to trigger copying of an object
     *
     * @param objectDescription XML representation of the object
     */
    void copyObject(std::string objectDescription);

    /**
     * @brief slot to signal the discovery of an error
     *
     * @param message error message
     */
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
