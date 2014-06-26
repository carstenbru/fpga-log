#include "configobjectdialog.h"
#include "ui_configobjectdialog.h"
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QFormLayout>
#include "spmcperipheral.h"

using namespace std;

ConfigObjectDialog::ConfigObjectDialog(QWidget *parent, CObject *object, DataLogger* dataLogger) :
    QDialog(parent),
    ui(new Ui::ConfigObjectDialog),
    object(object),
    dataLogger(dataLogger)
{
    ui->setupUi(this);


    QWidget * contents = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop);
    contents->setLayout(layout);
    ui->scrollArea->setWidget(contents);

    addNameGroup(layout);
    addHardwareParametersGroup(layout);
    addReqParametersGroup(layout);

    connect(this, SIGNAL(finished(int)), this, SLOT(storeParams()));
}

ConfigObjectDialog::~ConfigObjectDialog() {
    delete ui;
}

QFormLayout* ConfigObjectDialog::addGroup(QLayout* layout, string title) {
    QGroupBox* group = new QGroupBox();
    QFormLayout* groupLayout = new QFormLayout();
    group->setLayout(groupLayout);
    group->setTitle(QString().fromUtf8(title.c_str()));
    layout->addWidget(group);

    return groupLayout;
}

void ConfigObjectDialog::addNameGroup(QLayout* parent) {
    QFormLayout* layout = addGroup(parent, "Objekt");
    layout->addRow("Typ", new QLabel(QString(object->getType()->getDisplayName().c_str())));

    objectName.setText(QString(object->getName().c_str()));
    layout->addRow("Objektname", &objectName);

    connect(&objectName, SIGNAL(editingFinished()), this, SLOT(nameEdited()));
}

void ConfigObjectDialog::addPeripheralParameters(QFormLayout *parent, SpmcPeripheral* peripheral) {
    std::list<CParameter*> parameters = peripheral->getParameters();
    for (std::list<CParameter*>::iterator i = parameters.begin(); i != parameters.end(); i++) {
        string paramName = (*i)->getName();
        QWidget* widget = (*i)->getDataType()->getConfigWidget(dataLogger, (*i)->getValue());
        parent->addRow(paramName.c_str(), widget);
        paramWidgets[*i] = widget;
    }
}

void ConfigObjectDialog::addHardwareParametersGroup(QLayout *parent) {
    list<SpmcPeripheral*> peripherals = object->getPeripherals();
    list<SpmcPeripheral*>::iterator i = peripherals.begin();
    if (i != peripherals.end()) {
        QFormLayout* layout = addGroup(parent, "Hardware Parameter");
        for (; i != peripherals.end(); i++) {
                addPeripheralParameters(layout, *i);
        }
    }
}

void ConfigObjectDialog::addReqParametersGroup(QLayout *parent) {
    list<CParameter>* parameters = object->getInitMethod()->getParameters();
    list<CParameter>::iterator i = parameters->begin();
    i++;
    QFormLayout* layout = NULL;
    for (; i != parameters->end(); i++) {
        DataType* type = (*i).getDataType();
        if (!type->hasSuffix("_regs_t")) {
            if (layout == NULL) {
                layout = addGroup(parent, "benötigte Parameter");
            }

            QWidget* widget = type->getConfigWidget(dataLogger, (*i).getValue());
            layout->addRow(( *i).getName().c_str(), widget);
            paramWidgets[&*i] = widget;
        }
    }
}

void ConfigObjectDialog::nameEdited() {
    dataLogger->changeObjectName(object, objectName.text().toStdString());
}

void ConfigObjectDialog::storeParams() {
    for (map<CParameter*, QWidget*>::iterator i = paramWidgets.begin(); i != paramWidgets.end(); i++) {
        CParameter* param = i->first;
        param->setValue(param->getDataType()->getConfigData(i->second));
    }
}
