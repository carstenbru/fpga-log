#include "pinbox.h"
#include <list>
#include "datatype.h"

using namespace std;

PinBox::PinBox(QWidget *parent) :
    QComboBox(parent)
{
    setSizeAdjustPolicy(QComboBox::AdjustToContents);

    connect(this, SIGNAL(currentIndexChanged(QString)), this, SLOT(indexChanged()));
}

void PinBox::setPinItems(const QString& pinGroup) {
    clear();
    try {
        list<Pin> pinGroupList = DataTypePin::getPinType()->getPinsInGroup(pinGroup.toStdString());
        for (list<Pin>::iterator i = pinGroupList.begin(); i != pinGroupList.end(); i++) {
            addItem(QString((*i).getName().c_str()));
        }
    } catch (exception) {

    }
}

void PinBox::indexChanged() {
    if (count())
        emit pinChanged();
}
