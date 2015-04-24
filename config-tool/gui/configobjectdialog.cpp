#include "configobjectdialog.h"
#include "ui_configobjectdialog.h"
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QFormLayout>
#include <QInputDialog>
#include "spmcperipheral.h"

using namespace std;

ConfigObjectDialog::ConfigObjectDialog(QWidget *parent, CObject *object, DataLogger* dataLogger) :
    QDialog(parent),
    ui(new Ui::ConfigObjectDialog),
    object(object),
    dataLogger(dataLogger),
    signalMapper(NULL)
{
    ui->setupUi(this);

    setupUi();

    connect(this, SIGNAL(finished(int)), this, SLOT(storeParams()));
    connect(dataLogger, SIGNAL(criticalParameterChanged()), this, SLOT(reload()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteModule()));
}

ConfigObjectDialog::~ConfigObjectDialog() {
    delete ui;
    if (signalMapper != NULL)
        signalMapper->deleteLater();
}

void ConfigObjectDialog::setupUi() {
    paramWidgets.clear();

    QWidget * contents = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop);
    contents->setLayout(layout);
    ui->scrollArea->setWidget(contents);

    addNameGroup(layout);
    addHardwareParametersGroup(layout);
    addReqParametersGroup(layout);
    addAdvancedConfigGroup(layout);
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
    QVBoxLayout* pinLayout = new QVBoxLayout();
    for (list<SpmcPeripheral*>::iterator i = peripherals.begin(); i != peripherals.end(); i++) {
        if (!((*i)->getParameters().empty())) {
            addParameters(paramsLayout, (*i)->getParameters());
        }
        map<string, list<PeripheralPort*> > ports = (*i)->getPorts();

        for (map<string, list<PeripheralPort*> >::iterator i = ports.begin(); i != ports.end(); i++) {
            addPortsGroup(pinLayout, i->first, i->second);
        }
    }
    addGroup(layout, "Pins", pinLayout);

    QWidget* tpWidget = new QWidget();
    QFormLayout* tpLayout = new QFormLayout();
    tpWidget->setLayout(tpLayout);
    addParameters(tpLayout, object->getTimestampPinParameters());
    pinLayout->addWidget(tpWidget);

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
            if (!(*i).getHideFromUser()) {
                QWidget* widget = type->getConfigWidget(dataLogger, &*i);
                layout->addRow(( *i).getName().c_str(), widget);
                paramWidgets[&*i] = widget;
            }
        }
    }
    addGroup(parent, "benötigte Parameter", layout);
}

void ConfigObjectDialog::addAdvancedConfigGroup(QLayout *parent) {
    if (!getAdvancedConfigMethods().empty()) {
        QVBoxLayout* layout = new QVBoxLayout();
        QPushButton* addBtn = new QPushButton(QIcon::fromTheme("list-add"), QString::fromUtf8("Hinzufügen"));
        connect(addBtn, SIGNAL(clicked()), this, SLOT(addAdvancedConfig()));
        layout->addWidget(addBtn);
        addGroup(parent, "erweiterte Konfiguration", layout);

        signalMapper = new QSignalMapper(this);
        connect(signalMapper, SIGNAL(mapped(int)), object, SLOT(removeAdvancedConfig(int)));

        list<CMethod*> methods = object->getAdvancedConfig();
        int id = 0;
        for (list<CMethod*>::iterator i = methods.begin(); i != methods.end(); i++) {
            QPushButton* delBtn = new QPushButton(QIcon::fromTheme("list-remove"), QString::fromUtf8("Entfernen"));
            signalMapper->setMapping(delBtn, id++);
            connect(delBtn, SIGNAL(clicked()), signalMapper, SLOT(map()));

            QFormLayout* methodGroupLayout = new QFormLayout();
            methodGroupLayout->addRow(delBtn);

            string name = (*i)->getName();
            addParameters(methodGroupLayout, (*i)->getMethodParameterPointers());
            addGroup(layout, name, methodGroupLayout);
        }
    }
}

void ConfigObjectDialog::nameEdited() {
    dataLogger->changeObjectName(object, objectName->text().toStdString());
}

void ConfigObjectDialog::storeParams() {
    for (map<CParameter*, QWidget*>::iterator i = paramWidgets.begin(); i != paramWidgets.end(); i++) {
        try {
            CParameter* param = i->first;
            param->setValue(param->getDataType()->getConfigData(i->second));
        } catch (exception e) {

        }
    }
}

void ConfigObjectDialog::reload() {
    storeParams();

    QEvent *event = new QEvent((QEvent::Type)ConfigObjectDialogReloadEvent);
    QCoreApplication::postEvent(this, event);
}

QStringList ConfigObjectDialog::getAdvancedConfigMethods() {
    list<CMethod*> methods = object->getType()->getMethods();
    QStringList items;
    for (list<CMethod*>::iterator i = methods.begin(); i != methods.end(); i++) {
        CMethod* m = *i;
        if (!m->getHideFromUser())
            items.append(m->getName().c_str());
    }
    return items;
}

void ConfigObjectDialog::addAdvancedConfig() {
    QInputDialog dialog(this);
    dialog.setOption(QInputDialog::UseListViewForComboBoxItems);
    dialog.setWindowIcon(QIcon::fromTheme("list-add"));
    dialog.setWindowTitle(QString::fromUtf8("Hinzufügen"));
    dialog.setLabelText(QString::fromUtf8("verfügbare Methoden:"));

    QStringList items = getAdvancedConfigMethods();
    dialog.setComboBoxItems(items);
    if (dialog.exec() == QDialog::Accepted) {
        object->addAdvancedConfig(dialog.textValue().toStdString());
        reload();
    }
}

void ConfigObjectDialog::deleteModule() {
    done(DeleteResult);
}

bool ConfigObjectDialog::event(QEvent *event) {
    if (event->type() == ConfigObjectDialogReloadEvent) {
        signalMapper->deleteLater();
        setupUi();
        return true;
    }
    return QDialog::event(event);
}
