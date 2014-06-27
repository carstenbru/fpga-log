#include "datatype.h"
#include <iostream>
#include <QSpinBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include "datalogger.h"
#include "pinbox.h"

using namespace std;

std::map<std::string, DataType*> DataType::types;
std::map<std::string, DataTypeStruct*> DataTypeStruct::types;

DataTypePin DataTypePin::pinType("pin");

DataType::DataType(std::string name) :
    name(name)
{
    types[name] = this;
}

std::string DataType::getCleanedName() {
    std::string s = name;
    size_t i = s.rfind("_t");
    if (i < s.length())
        s.erase(i, s.length());
    return s;
}

QWidget* DataType::getConfigWidget(DataLogger*, string) {
    cerr << "Konfigurationselement fuer nicht konfigurierbaren Typ angefragt: " << name << endl;
    return NULL;
}

DataTypeStruct::DataTypeStruct(std::string name) :
    DataType(name),
    super(NULL)
{
    types[name] = this;
}

bool DataTypeStruct::isInstantiableObject() {
    return (getMethod("init") != NULL);
}

bool DataTypeStruct::isSuperclass(DataTypeStruct* dataType) {
    if (dataType == this)
        return true;
    else {
        if (super != NULL)
            return super->isSuperclass(dataType);
        else
            return false;
    }
}

CMethod* DataTypeStruct::getMethod(std::string methodName) {
    for (list<CMethod*>::iterator i = methods.begin(); i != methods.end(); i++) {
        if ((*i)->getName().compare(methodName) == 0)
            return *i;
    }
    return NULL;
}

QWidget* DataTypeStruct::getConfigWidget(DataLogger* dataLogger, string startValue) {
    QComboBox* cbox = new QComboBox();
    list<CObject*> instances = dataLogger->getInstances(this);
    for (list<CObject*>::iterator i = instances.begin(); i != instances.end(); i++) {
        cbox->addItem(QString((*i)->getName().c_str()));
    }
    cbox->setCurrentIndex(cbox->findText(QString(startValue.c_str())));
    return cbox;
}

std::string DataTypeStruct::getConfigData(QWidget* widget) {
    QComboBox* cbox = dynamic_cast<QComboBox*>(widget);
    return cbox->currentText().toStdString();
}

DataTypeNumber::DataTypeNumber(std::string name, long min, long max) :
    DataType(name),
    min(min),
    max(max)
{
}

QWidget* DataTypeNumber::getConfigWidget(DataLogger*, string startValue) {
    QSpinBox* sbox = new QSpinBox();
    int minInt = (min < numeric_limits<int>::min()) ? numeric_limits<int>::min() : min; //TODO implement subclass of QAbstractSpinBox with long int support
    int maxInt = (max > numeric_limits<int>::max()) ? numeric_limits<int>::max() : max;
    sbox->setMinimum(minInt);
    sbox->setMaximum(maxInt);
    long value = atol(startValue.c_str());
    sbox->setValue(value);
    return sbox;
}

std::string DataTypeNumber::getConfigData(QWidget* widget) {
    QSpinBox* sbox = dynamic_cast<QSpinBox*>(widget);
    return std::to_string(sbox->value());
}

DataTypeEnumeration::DataTypeEnumeration(std::string name) :
    DataType(name)
{
}

QWidget* DataTypeEnumeration::getConfigWidget(DataLogger*, std::string startValue) {
    return getConfigBox(startValue);
}

QComboBox* DataTypeEnumeration::getConfigBox(std::string startValue) {
    QComboBox* cbox = new QComboBox();
    for (list<string>::iterator i = values.begin(); i != values.end(); i++) {
        cbox->addItem(QString((*i).c_str()));
    }
    cbox->setCurrentIndex(cbox->findText(QString(startValue.c_str())));
    return cbox;
}

std::string DataTypeEnumeration::getConfigData(QWidget* widget) {
    QComboBox* cbox = dynamic_cast<QComboBox*>(widget);
    return cbox->currentText().toStdString();
}

DataTypeString::DataTypeString(std::string name) :
    DataType(name)
{
}

QWidget* DataTypeString::getConfigWidget(DataLogger*, std::string startValue) {
    QLineEdit* lEdit = new QLineEdit();
    lEdit->setText(startValue.c_str());
    return lEdit;
}

std::string DataTypeString::getConfigData(QWidget* widget) {
    QLineEdit* lEdit = dynamic_cast<QLineEdit*>(widget);
    return lEdit->text().toStdString();
}

DataTypePin::DataTypePin(std::string name) :
    DataType(name)
{

}

void DataTypePin::addPin(std::string groupName, Pin& pin) {
    pins[groupName].push_back(pin);
}

QWidget* DataTypePin::getConfigWidget(DataLogger*, std::string startValue) {
    size_t d = startValue.find(":");
    string groupName = startValue;
    string pinName = startValue;
    if (d < startValue.length()) {
        groupName.erase(d, groupName.length());
        pinName.erase(0, d + 1);
    }

    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setMargin(0);
    widget->setLayout(layout);

    QComboBox* groupBox = new QComboBox();
    layout->addWidget(groupBox);
    for (map<string, list<Pin> >::iterator i = pins.begin(); i != pins.end(); i++) {
        groupBox->addItem(QString(i->first.c_str()));
    }
    groupBox->setCurrentIndex(groupBox->findText(QString(groupName.c_str())));

    PinBox* pinBox = new PinBox();
    layout->addWidget(pinBox);
    pinBox->setPinItems(QString(groupName.c_str()));
    pinBox->setCurrentIndex(pinBox->findText(QString(pinName.c_str())));

    QObject::connect(groupBox, SIGNAL(currentIndexChanged(QString)), pinBox, SLOT(setPinItems(QString)));

    return widget;
}

std::string DataTypePin::getConfigData(QWidget* widget) {
    QComboBox* groupBox = dynamic_cast<QComboBox*>(widget->children().at(1));
    QComboBox* pinBox = dynamic_cast<QComboBox*>(widget->children().at(2));

    return groupBox->currentText().toStdString() + ":" + pinBox->currentText().toStdString();
}
