/**
 * @file configobjectdialog.h
 * @brief ConfigObjectDialog GUI class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef CONFIGOBJECTDIALOG_H
#define CONFIGOBJECTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QFormLayout>
#include <map>
#include <list>
#include <string>
#include <QSignalMapper>
#include "cobject.h"
#include "datalogger.h"

namespace Ui {
class ConfigObjectDialog;
}
class MainWindow;

// custom event code indicating a neccessary reload of the dialog
#define ConfigObjectDialogReloadEvent 1774

/**
 * @brief datalogger object configuration dialog
 *
 * This dialog lets the user edit all configurable parameters of an object and also copy or delete it.
 */
class ConfigObjectDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief enumeration of possible (custom) dialog result codes
     */
    enum DialogCode { DeleteResult = 2 /**< delete the object after closing the dialog */};

    /**
     * @brief constuctor
     *
     * @param parent parent window
     * @param object object which should be configured
     * @param dataLogger datalogger the object belongs to
     */
    explicit ConfigObjectDialog(MainWindow *parent, CObject* object, DataLogger *dataLogger);
    ~ConfigObjectDialog();
private:
    /**
     * @brief adds a configuration group to the window (e.g. object, pins, required parameters)
     *
     * It the group is empty it will not be added but deleted.
     *
     * @param parentLayout parent layout
     * @param title title of the new group
     * @param toolTip tool-tip text describing the group
     * @param groupLayout layout to add containing the group
     */
    void addGroup(QLayout* parentLayout, std::string title, std::string toolTip, QLayout* groupLayout);
    /**
     * @brief adds a configuration group to the window (e.g. object, pins, required parameters) without a tool-tip
     *
     * @param parentLayout parent layout
     * @param title title of the new group
     * @param groupLayout layout to add containing the group
     */
    void addGroup(QLayout* parentLayout, std::string title, QLayout* groupLayout) { addGroup(parentLayout, title, "", groupLayout); }
    /**
     * @brief creates and adds the object name group
     *
     * @param parent parent layout
     */
    void addNameGroup(QLayout *parent);
    /**
     * @brief creates and adds the configuration group for a single port
     *
     * @param parent parent layout
     * @param groupName port group name
     * @param ports ports to add
     */
    void addPortsGroup(QLayout *parent, std::string groupName, std::list<PeripheralPort*>& ports);
    /**
     * @brief adds configuration input fields for a list of parameters
     *
     * @param parent parent layout
     * @param parameters list of parameters to add
     */
    void addParameters(QFormLayout *parent, std::list<CParameter *> parameters);
    /**
     * @brief adds configuration input fields for a list of parameters
     *
     * @param parentMethod parent method of the parameters
     * @param parent parent layout
     * @param parameters list of parameters to add
     */
    void addParameters(CMethod *parentMethod, QFormLayout *parent, std::list<CParameter *> parameters);
    /**
     * @brief creates and adds a group for hardware parameters (e.g. pins, baudrate,...)
     *
     * @param parent parent layout
     */
    void addHardwareParametersGroup(QLayout *parent);
    /**
     * @brief creates and adds a group for required parameters (i.e. for the parameters of the init function)
     *
     * @param parent parent layout
     */
    void addReqParametersGroup(QLayout *parent);
    /**
     * @brief creates and adds a group for advanced parameters (i.e. for the parameters of added method calls)
     *
     * @param parent parent layout
     */
    void addAdvancedConfigGroup(QLayout *parent);

    bool event(QEvent *event);

    /**
     * @brief gets a list of configured method calls (advanced object configuration)
     *
     * @return list of methods to add
     */
    QStringList getAdvancedConfigMethods();

    Ui::ConfigObjectDialog *ui; /**< UI */

    CObject* object; /**< object to configure */
    DataLogger* dataLogger; /**< parent datalogger */
    QLineEdit* objectName; /**< object name input field */

    QSpinBox* spartanMcCoreEdit; /**< SpartanMC core number input field */

    std::map<CParameter*, QWidget*> paramWidgets; /**< parameter input fields (key: parameter, value: input widget */
    std::map<CParameter*, DataType*> paramTypes; /**< parameter types (key: parameter, value: data type */

    QSignalMapper* signalMapper; /**< signal mapper to trigger re-drawing of the dialog after all deletes are finished */

    CParameter* objectRequestedForParam; /**< parameter to store the parameter an object creation was requested for */
signals:
    /**
     * @brief emitted when a new object of a specific type should be created
     */
    void newObjectRequest(DataTypeStruct*,ConfigObjectDialog*);

    /**
     * @brief emitted when the object should be copied
     *
     * @param objectDescription XML representation of the object
     */
    void copyObject(std::string objectDescription);
private slots:
    /**
     * @brief slot to notify about a change in the name input field
     */
    void nameEdited();
    /**
     * @brief slot to trigger storing of all parameters
     */
    void storeParams();
    /**
     * @brief slot to trigger creation of the UI
     */
    void setupUi();
    /**
     * @brief slot to indicate a needed reload of the dialog
     */
    void reload();

    /**
     * @brief slot to notify about an added method call (advanced config)
     */
    void addAdvancedConfig();

    /**
     * @brief slot to trigger deletion of the edited object
     */
    void deleteModule();

    /**
     * @brief slot to notify about an object selection change in one of the parameter fields
     *
     * This is neccassary as these fields have the option to request the generation of a new object,
     * so we have to check for that.
     */
    void objectSelectionChanged(QString);

    /**
     * @brief copy object request
     */
    void copyObjectButton();
public slots:
    /**
     * @brief slot to receive message of object creation finished
     * @see newObjectRequest
     *
     * @param name name of the new object
     */
    void objectCreationFinished(std::string name);
};

#endif // CONFIGOBJECTDIALOG_H
