/**
 * @file targetconfigdialog.cpp
 * @brief TargetConfigDialog GUI class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include "targetconfigdialog.h"
#include "ui_targetconfigdialog.h"
#include <iostream>
#include <QMessageBox>
#include <QSpinBox>

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
    layout->addRow(tr("Input clock"), clockFreqWidget);

    QSpinBox* freqSpinBox =(QSpinBox*)clockFreqWidget;
    connect(freqSpinBox, SIGNAL(valueChanged(int)), this, SLOT(freqChanged(int)));

    systemClkSelect = new QComboBox();
    int clockFreqValue = atoi(clockFreq->getValue().c_str());
    freqChanged(clockFreqValue);

    layout->addRow(tr("System clock frequency"), systemClkSelect);

    CParameter* expertMode = dataLogger->getExpertMode();
    expertModeWidget = expertMode->getDataType()->getConfigWidget(dataLogger, expertMode);
    layout->addRow(tr("Expert mode"), expertModeWidget);
}

void TargetConfigDialog::targetChanged(QString newTarget) {
    dataLogger->getTarget()->setValue(newTarget.toStdString());
    dataLogger->loadTargetPins();

    dataLogger->getClockPin()->setValue("");

    widget->deleteLater();
    generateUi();
}

void TargetConfigDialog::freqChanged(int clockFreqValue) {
    systemClkSelect->clear();

    int curSysFreq = clockFreqValue / dataLogger->getClkDivide() * dataLogger->getClkMultiply();
    int curSysFreqItem = -1;
    frequencies.clear();
    freqCoefficients.clear();
    DataTypeNumber* divType = (DataTypeNumber*)DataType::getType("sysclk_regs_t_CLKFX_DIVIDE");
    DataTypeNumber* multType = (DataTypeNumber*)DataType::getType("sysclk_regs_t_CLKFX_MULTIPLY");
    for (int divider = divType->getMax(); divider >= divType->getMin(); divider--) {
        for (int multiplier = multType->getMax(); multiplier >= multType->getMin(); multiplier--) {
            int f = clockFreqValue / divider * multiplier;
            frequencies.insert(f);
            freqCoefficients[f] = pair<int, int>(divider, multiplier);
        }
    }
    int itemCount = 0;
    for (set<int>::iterator i = frequencies.begin(); i != frequencies.end(); i++) {
        int freq = *i;
        if (freq == curSysFreq) {
            curSysFreqItem = itemCount;
        }
        itemCount++;
        bool onlyMhz = ((freq % 1000000) == 0);
        bool recommendedFreq = ((freq % 2000) == 0);
        string s = "";
        if (onlyMhz) {
            s += to_string(freq / 1000000) + " MHz";
        } else {
            s += to_string(freq / 1000000.0f) + " MHz";
        }
        if (!recommendedFreq) {
            s = "(" + s + ")";
        }
        systemClkSelect->addItem(s.c_str());
    }
    systemClkSelect->setCurrentIndex(curSysFreqItem);
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

    int itemNr = systemClkSelect->currentIndex();

    set<int>::iterator i = frequencies.begin();
    while (itemNr--) {
        i++;
    }
    int freq = *i;
    pair<int, int> c = freqCoefficients.at(freq);
    dataLogger->getClockDivideParam()->setValue(to_string(c.first));
    dataLogger->getClockMultiplyParam()->setValue(to_string(c.second));

    CParameter* expertMode = dataLogger->getExpertMode();
    expertMode->setValue(expertMode->getDataType()->getConfigData(expertModeWidget));

    if (systemClkSelect->currentText().startsWith("(")) {
        QString text = tr("You chose a not recommended clock frequency. This could lead to unexpected behaviour. Use this frequency only if you are sure what you are doing!");
        cerr << tr("WARNING:").toStdString() << " " << text.toStdString() << endl;
        QMessageBox dialog(QMessageBox::Warning,
                           tr("not recommended clock frequency"),
                           text,
                           QMessageBox::Ok);
        dialog.exec();
    }
}
