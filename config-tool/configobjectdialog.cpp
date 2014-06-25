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

    addNameGroup(addGroup(layout, "Objekt"));
    addReqParametersGroup(addGroup(layout, "benötigte Parameter"));
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

void ConfigObjectDialog::addNameGroup(QFormLayout* layout) {
    layout->addRow("Typ", new QLabel(QString(object->getType()->getDisplayName().c_str())));

    objectName.setText(QString(object->getName().c_str()));
    layout->addRow("Objektname", &objectName);

    connect(&objectName, SIGNAL(editingFinished()), this, SLOT(nameEdited()));
}

void ConfigObjectDialog::addReqParametersGroup(QFormLayout* layout) {

}

void ConfigObjectDialog::nameEdited() {
    dataLogger->changeObjectName(object, objectName.text().toStdString());
}
