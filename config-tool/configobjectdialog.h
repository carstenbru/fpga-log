#ifndef CONFIGOBJECTDIALOG_H
#define CONFIGOBJECTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
#include <map>
#include <list>
#include <string>
#include "cobject.h"
#include "datalogger.h"

namespace Ui {
class ConfigObjectDialog;
}

class ConfigObjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigObjectDialog(QWidget *parent, CObject* object, DataLogger *dataLogger);
    ~ConfigObjectDialog();
private:
    void addGroup(QLayout* parentLayout, std::string title, QLayout* groupLayout);
    void addNameGroup(QLayout *parent);
    void addPeripheralParameters(QFormLayout *parent, SpmcPeripheral *peripheral);
    void addHardwareParametersGroup(QLayout *parent);
    void addReqParametersGroup(QLayout *parent);

    Ui::ConfigObjectDialog *ui;

    CObject* object;
    DataLogger *dataLogger;
    QLineEdit objectName;

    std::map<CParameter*, QWidget*> paramWidgets;
private slots:
    void nameEdited();
    void storeParams();
};

#endif // CONFIGOBJECTDIALOG_H
