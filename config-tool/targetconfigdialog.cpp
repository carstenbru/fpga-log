#include "targetconfigdialog.h"
#include "ui_targetconfigdialog.h"

TargetConfigDialog::TargetConfigDialog(QWidget *parent, DataLogger* dataLogger) :
    QDialog(parent),
    ui(new Ui::TargetConfigDialog),
    dataLogger(dataLogger)
{
    ui->setupUi(this);

    generateUi();

    connect(this, SIGNAL(finished(int)), this, SLOT(storeParams()));
}

TargetConfigDialog::~TargetConfigDialog()
{
    delete ui;
}

void TargetConfigDialog::generateUi() {
    widget = new QWidget();
    ui->verticalLayout->insertWidget(0, widget);

    QFormLayout* layout = new QFormLayout();
    widget->setLayout(layout);

    CParameter* target = dataLogger->getTarget();
    QComboBox* targetBox = dynamic_cast<DataTypeEnumeration*>(target->getDataType())->getConfigBox(target->getValue());
    layout->addRow("Zielplattform", targetBox);
    connect(targetBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(targetChanged(QString)));

    CParameter* clockPin = dataLogger->getClockPin();
    clockPinWidget = clockPin->getDataType()->getConfigWidget(dataLogger, clockPin->getValue());
    layout->addRow("Clock Pin", clockPinWidget);
}

void TargetConfigDialog::targetChanged(QString newTarget) {
    dataLogger->getTarget()->setValue(newTarget.toStdString());
    dataLogger->loadTargetPins();

    widget->deleteLater();
    generateUi();
}

void TargetConfigDialog::storeParams() {
    CParameter* clkParam = dataLogger->getClockPin();
    clkParam->setValue(clkParam->getDataType()->getConfigData(clockPinWidget));
}
