#include "pinbox.h"
#include <list>
#include "datatype.h"

using namespace std;

PinBox::PinBox(DataLogger *dataLogger, string selectedPin, QWidget *parent) :
    QComboBox(parent),
    dataLogger(dataLogger),
    selectedPin(selectedPin)
{
    setSizeAdjustPolicy(QComboBox::AdjustToContents);

    connect(this, SIGNAL(currentIndexChanged(QString)), this, SLOT(indexChanged()));
}

bool PinBox::pinAssigned(list<string[4]>& pinAssignments, string name) {
    for (list<string[4]>::iterator pin = pinAssignments.begin(); pin != pinAssignments.end(); pin++) {
         string pinArray[4] = *pin;
         if (pinArray[0].compare(name) == 0) {
             return true;
         }
    }
    return false;
}

void PinBox::setPinItems(const QString& pinGroup) {
    clear();
    try {
        list<string[4]> pinAssignments = dataLogger->getPinAssignments();

        list<Pin> pinGroupList = DataTypePin::getPinType()->getPinsInGroup(pinGroup.toStdString());
        for (list<Pin>::iterator i = pinGroupList.begin(); i != pinGroupList.end(); i++) {
            string pinFullName = pinGroup.toStdString() + ":" + (*i).getName();
            if ((pinFullName.compare(selectedPin) == 0) || !pinAssigned(pinAssignments, pinFullName)) {
              addItem(QString((*i).getName().c_str()));
            }
        }
    } catch (exception) {

    }
}

void PinBox::indexChanged() {
    if (count())
        emit pinChanged();
}
