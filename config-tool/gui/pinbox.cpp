/**
 * @file pinbox.cpp
 * @brief PinBox class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include "pinbox.h"
#include <list>
#include "datatype.h"

using namespace std;

PinBox::PinBox(DataLogger *dataLogger, string selectedPin, QWidget *parent) :
    QComboBox(parent),
    dataLogger(dataLogger),
    selectedPin(selectedPin),
    disablePinChange(false)
{
    setSizeAdjustPolicy(QComboBox::AdjustToContents);

    connect(this, SIGNAL(currentIndexChanged(QString)), this, SLOT(indexChanged()));
}

bool PinBox::pinAssigned(map<CParameter*, string[4]>& pinAssignments, string name) {
    for (map<CParameter*, string[4]>::iterator pin = pinAssignments.begin(); pin != pinAssignments.end(); pin++) {
         string pinArray[4] = pin->second;
         if (pinArray[0].compare(name) == 0) {
             return true;
         }
    }
    return false;
}

void PinBox::setPinItems() {
    setPinItems(pinGroup.c_str());
}

void PinBox::setPinItems(const QString& pinGroup) {
    disablePinChange = true;
    this->pinGroup = pinGroup.toStdString();
    clear();
    try {
        map<CParameter*, string[4]> pinAssignments = dataLogger->getPinAssignments();

        list<Pin> pinGroupList = DataTypePin::getPinType()->getPinsInGroup(pinGroup.toStdString());
        for (list<Pin>::iterator i = pinGroupList.begin(); i != pinGroupList.end(); i++) {
            string pinFullName = pinGroup.toStdString() + ":" + (*i).getName();
            if (((*i).getName().compare(selectedPin) == 0) || !pinAssigned(pinAssignments, pinFullName)) {
              addItem(QString((*i).getName().c_str()));
            }
        }
    } catch (exception) {
    }
    int ind = findText(QString(selectedPin.c_str()));
    if (ind >= 0) {
      setCurrentIndex(ind);
      disablePinChange = false;
    } else {
      disablePinChange = false;
      indexChanged();
    }
}

void PinBox::indexChanged() {
    if (!disablePinChange && count()) {
        selectedPin = currentText().toStdString();
        emit pinChanged();
    }
}
