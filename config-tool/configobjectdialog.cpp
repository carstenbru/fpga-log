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

void ConfigObjectDialog::addGroup(QLayout* parentLayout, string title, QLayout *groupLayout) {
    QGroupBox* group = new QGroupBox();
    group->setLayout(groupLayout);
    group->setTitle(QString().fromUtf8(title.c_str()));
    if (!groupLayout->isEmpty()) {
        parentLayout->addWidget(group);
    } else {
        group->deleteLater();
    }
}

void ConfigObjectDialog::addNameGroup(QLayout* parent) {
    QFormLayout* layout = new QFormLayout();
    layout->addRow("Typ", new QLabel(QString(object->getType()->getDisplayName().c_str())));

    objectName.setText(QString(object->getName().c_str()));
    layout->addRow("Objektname", &objectName);

    connect(&objectName, SIGNAL(editingFinished()), this, SLOT(nameEdited()));

    addGroup(parent, "Objekt", layout);
}

void ConfigObjectDialog::addPeripheralParameters(QFormLayout *parent, SpmcPeripheral* peripheral) {
    std::list<CParameter*> parameters = peripheral->getParameters();
    for (std::list<CParameter*>::iterator i = parameters.begin(); i != parameters.end(); i++) {
        if (!(*i)->getHideFromUser()) {
            string paramName = (*i)->getName();
            QWidget* widget = (*i)->getDataType()->getConfigWidget(dataLogger, (*i)->getValue());
            parent->addRow(paramName.c_str(), widget);
            paramWidgets[*i] = widget;
        }
    }
}

void ConfigObjectDialog::addHardwareParametersGroup(QLayout *parent) {
    list<SpmcPeripheral*> peripherals = object->getPeripherals();

    QFormLayout* layout = new QFormLayout();
    for (list<SpmcPeripheral*>::iterator i = peripherals.begin(); i != peripherals.end(); i++) {
        if (!((*i)->getParameters().empty())) {
            addPeripheralParameters(layout, *i);
        }
    }
    addGroup(parent, "Hardware Parameter", layout);
}

void ConfigObjectDialog::addReqParametersGroup(QLayout *parent) {
    list<CParameter>* parameters = object->getInitMethod()->getParameters();
    list<CParameter>::iterator i = parameters->begin();
    i++;
    QFormLayout* layout = new QFormLayout();
    for (; i != parameters->end(); i++) {
        DataType* type = (*i).getDataType();
        if (!type->hasSuffix("_regs_t")) {
            QWidget* widget = type->getConfigWidget(dataLogger, (*i).getValue());
            layout->addRow(( *i).getName().c_str(), widget);
            paramWidgets[&*i] = widget;
        }
    }
    addGroup(parent, "benötigte Parameter", layout);
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
