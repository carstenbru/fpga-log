#include "targetconfigdialog.h"
#include "ui_targetconfigdialog.h"

using namespace std;

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
    QComboBox* targetBox = dynamic_cast<DataTypeEnumeration*>(target->getDataType())->getConfigBox(NULL, target);
    layout->addRow(target->getName().c_str(), targetBox);
    connect(targetBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(targetChanged(QString)));

    CParameter* clockPin = dataLogger->getClockPin();
    clockPinWidget = DataTypePin::getPinType()->getConfigWidget(dataLogger, clockPin, this, SLOT(pinChanged()));
    layout->addRow(clockPin->getName().c_str(), clockPinWidget);

    CParameter* clockFreq = dataLogger->getClockFreq();
    clockFreqWidget = clockFreq->getDataType()->getConfigWidget(dataLogger, clockFreq);
    layout->addRow(clockFreq->getName().c_str(), clockFreqWidget);
}

void TargetConfigDialog::targetChanged(QString newTarget) {
    dataLogger->getTarget()->setValue(newTarget.toStdString());
    dataLogger->loadTargetPins();

    dataLogger->getClockPin()->setValue("");

    widget->deleteLater();
    generateUi();
}

void TargetConfigDialog::pinChanged() {
    CParameter* clkParam = dataLogger->getClockPin();
    string clkPin = clkParam->getDataType()->getConfigData(clockPinWidget);
    clkParam->setValue(clkPin);

    Pin* freqPin = DataTypePin::getPinType()->getPin(Pin::getGroupFromFullName(clkPin), Pin::getPinFromFullName(clkPin));
    if (freqPin != NULL) {
        string freq = freqPin->getFreq();
        if (!freq.empty()) {
            dataLogger->getClockFreq()->setValue(freq);
        }
    }

    widget->deleteLater();
    generateUi();
}

void TargetConfigDialog::storeParams() {
    CParameter* clkFreqParam = dataLogger->getClockFreq();
    clkFreqParam->setValue(clkFreqParam->getDataType()->getConfigData(clockFreqWidget));
}
