#include "configobjectdialog.h"
#include "ui_configobjectdialog.h"
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QFormLayout>

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
    addReqParametersGroup(layout);

    connect(this, SIGNAL(finished(int)), this, SLOT(storeReqParams()));
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

void ConfigObjectDialog::addReqParametersGroup(QLayout *parent) {
    list<CMethodParameter*> parameters = object->getType()->getMethod("init")->getParameters();
    list<CMethodParameter*>::iterator i = parameters.begin();
    i++;
    if (i != parameters.end()) {
        QFormLayout* layout = addGroup(parent, "benötigte Parameter");
        for (; i != parameters.end(); i++) {
            string paramName =( *i)->getName();
            QWidget* widget = (*i)->getDataType()->getConfigWidget(dataLogger, object->getReqParameter(paramName));
            layout->addRow(paramName.c_str(), widget);
            paramWidgets[*i] = widget;
        }
    }
}

void ConfigObjectDialog::nameEdited() {
    dataLogger->changeObjectName(object, objectName.text().toStdString());
}

void ConfigObjectDialog::storeReqParams() {
    for (map<CMethodParameter*, QWidget*>::iterator i = paramWidgets.begin(); i != paramWidgets.end(); i++) {
        CMethodParameter* param = i->first;
        object->setReqParameter(param->getName(), param->getDataType()->getConfigData(i->second));
    }
}
