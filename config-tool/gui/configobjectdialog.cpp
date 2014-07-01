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

    setupUi();

    connect(this, SIGNAL(finished(int)), this, SLOT(storeParams()));
    connect(dataLogger, SIGNAL(criticalParameterChanged()), this, SLOT(reload()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(reject()));
}

ConfigObjectDialog::~ConfigObjectDialog() {
    delete ui;
}

void ConfigObjectDialog::setupUi() {
    paramWidgets.clear();

    QWidget * contents = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop);
    contents->setLayout(layout);
    ui->scrollArea->setWidget(contents);

    connect(contents, SIGNAL(destroyed()), this, SLOT(setupUi()));

    addNameGroup(layout);
    addHardwareParametersGroup(layout);
    addReqParametersGroup(layout);
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

    objectName = new QLineEdit();
    objectName->setText(QString(object->getName().c_str()));
    layout->addRow("Objektname", objectName);

    connect(objectName, SIGNAL(editingFinished()), this, SLOT(nameEdited()));

    addGroup(parent, "Objekt", layout);
}

void ConfigObjectDialog::addPortsGroup(QLayout *parent, string groupName, list<PeripheralPort *>& ports) {
    QFormLayout* portGroupLayout = new QFormLayout();
    for (list<PeripheralPort*>::iterator i = ports.begin(); i != ports.end(); i++) {
        addParameters(portGroupLayout, (*i)->getLines());
    }
    addGroup(parent, groupName, portGroupLayout);
}

void ConfigObjectDialog::addParameters(QFormLayout *parent, std::list<CParameter*> parameters) {
    for (std::list<CParameter*>::iterator i = parameters.begin(); i != parameters.end(); i++) {
        if (!(*i)->getHideFromUser()) {
            string paramName = (*i)->getName();
            QWidget* widget = (*i)->getDataType()->getConfigWidget(dataLogger, *i);
            parent->addRow(paramName.c_str(), widget);
            paramWidgets[*i] = widget;
        }
    }
}

void ConfigObjectDialog::addHardwareParametersGroup(QLayout *parent) {
    list<SpmcPeripheral*> peripherals = object->getPeripherals();

    QVBoxLayout* layout = new QVBoxLayout();

    QWidget* widget = new QWidget();
    QFormLayout* paramsLayout = new QFormLayout();
    widget->setLayout(paramsLayout);
    for (list<SpmcPeripheral*>::iterator i = peripherals.begin(); i != peripherals.end(); i++) {
        if (!((*i)->getParameters().empty())) {
            addParameters(paramsLayout, (*i)->getParameters());
        }
        map<string, list<PeripheralPort*> > ports = (*i)->getPorts();
        QVBoxLayout* pinLayout = new QVBoxLayout();
        for (map<string, list<PeripheralPort*> >::iterator i = ports.begin(); i != ports.end(); i++) {
            addPortsGroup(pinLayout, i->first, i->second);
        }
        addGroup(layout, "Pins", pinLayout);
    }

    if (!paramsLayout->isEmpty()) {
        layout->insertWidget(0, widget);
    } else {
        widget->deleteLater();
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
            QWidget* widget = type->getConfigWidget(dataLogger, &*i);
            layout->addRow(( *i).getName().c_str(), widget);
            paramWidgets[&*i] = widget;
        }
    }
    addGroup(parent, "benötigte Parameter", layout);
}

void ConfigObjectDialog::nameEdited() {
    dataLogger->changeObjectName(object, objectName->text().toStdString());
}

void ConfigObjectDialog::storeParams() {
    for (map<CParameter*, QWidget*>::iterator i = paramWidgets.begin(); i != paramWidgets.end(); i++) {
        CParameter* param = i->first;
        param->setValue(param->getDataType()->getConfigData(i->second));
    }
}

void ConfigObjectDialog::reload() {
    storeParams();
    ui->scrollArea->widget()->deleteLater();
}
