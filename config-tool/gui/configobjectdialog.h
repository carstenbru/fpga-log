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
    void addGroup(QLayout* parentLayout, std::string title, std::string toolTip, QLayout* groupLayout);
    void addGroup(QLayout* parentLayout, std::string title, QLayout* groupLayout) { addGroup(parentLayout, title, "", groupLayout); }
    void addNameGroup(QLayout *parent);
    void addPortsGroup(QLayout *parent, std::string groupName, std::list<PeripheralPort*>& ports);
    void addParameters(QFormLayout *parent, std::list<CParameter *> parameters);
    void addParameters(CMethod *parentMethod, QFormLayout *parent, std::list<CParameter *> parameters);
    void addHardwareParametersGroup(QLayout *parent);
    void addReqParametersGroup(QLayout *parent);
    void addAdvancedConfigGroup(QLayout *parent);

    bool event(QEvent *event);

    QStringList getAdvancedConfigMethods();

    Ui::ConfigObjectDialog *ui;

    CObject* object;
    DataLogger* dataLogger;
    QLineEdit* objectName;

    QSpinBox* spartanMcCoreEdit;

    std::map<CParameter*, QWidget*> paramWidgets;
    std::map<CParameter*, DataType*> paramTypes;

    QSignalMapper* signalMapper;

    CParameter* objectRequestedForParam;
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
    void nameEdited();
    void storeParams();
    void setupUi();
    void reload();

    void addAdvancedConfig();

    void deleteModule();

    void objectSelectionChanged(QString);

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
