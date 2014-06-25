#ifndef CONFIGOBJECTDIALOG_H
#define CONFIGOBJECTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
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
    QFormLayout* addGroup(QLayout* layout, std::string title);
    void addNameGroup(QFormLayout* layout);
    void addReqParametersGroup(QFormLayout* layout);

    Ui::ConfigObjectDialog *ui;

    CObject* object;
    DataLogger *dataLogger;
    QLineEdit objectName;
private slots:
    void nameEdited();
};

#endif // CONFIGOBJECTDIALOG_H
