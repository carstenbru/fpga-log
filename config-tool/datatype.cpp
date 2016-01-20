#include "datatype.h"
#include <iostream>
#include <QSpinBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <sstream>
#include "datalogger.h"
#include "gui/pinbox.h"

using namespace std;

std::map<std::string, DataType*> DataType::types;
std::map<std::string, DataTypeStruct*> DataTypeStruct::types;
std::map<std::string, DataTypeFunction*> DataTypeFunction::types;

DataTypePin DataTypePin::pinType("pin", true);

DataTypeEnumeration DataTypeEnumeration::controlParameterType("parameter_type_t", true);

DataType::DataType(std::string name, bool globalType) :
    name(name),
    headerFile(""),
    globalType(globalType)
{
    types[name] = this;
}

DataType::DataType(std::string name, std::string headerFile, bool globalType) :
    name(name),
    headerFile(headerFile),
    globalType(globalType)
{
    types[name] = this;
}

void DataType::removeLocalTypes() {
    for (std::map<std::string, DataType*>::iterator i = types.begin(); i != types.end(); i++) {
        if (!(i->second->globalType)) {
            delete i->second;
        }
    }
}

bool DataType::hasSuffix(std::string suffix) {
    if (suffix.length() > name.length())
        return false;
    return (name.rfind(suffix) == (name.length() - suffix.length()));
}

std::string DataType::getCleanedName() {
    std::string s = name;
    size_t i = s.rfind("_t");
    if (i < s.length())
        s.erase(i, s.length());
    return s;
}

QWidget* DataType::getConfigWidget(DataLogger*, CParameter*) {
    cerr << "Konfigurationselement fuer nicht konfigurierbaren Typ angefragt: " << name << endl;
    return NULL;
}

DataTypeStruct::DataTypeStruct(std::string name, string headerFile, bool globalType, string description) :
    DataType(name, headerFile, globalType),
    super(NULL),
    description(description)
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

void DataTypeStruct::addMethod(CMethod* method, bool inherit) {
    methods.push_back(method);

    if (inherit) {
        for (list<DataTypeStruct*>::iterator i = childs.begin(); i != childs.end(); i++) {
            (*i)->addMethod(method, inherit);
        }
    }
}

QWidget* DataTypeStruct::getConfigWidget(DataLogger* dataLogger, CParameter *param) {
    QComboBox* cbox = new QComboBox();
    list<CObject*> instances = dataLogger->getInstances(this);
    for (list<CObject*>::iterator i = instances.begin(); i != instances.end(); i++) {
        cbox->addItem(QString((*i)->getName().c_str()));
    }
    cbox->addItem(NEW_OBJECT_STRING);
    cbox->setCurrentIndex(cbox->findText(QString(param->getValue().c_str())));
    return cbox;
}

std::string DataTypeStruct::getConfigData(QWidget* widget) {
    QComboBox* cbox = dynamic_cast<QComboBox*>(widget);
    return cbox->currentText().toStdString();
}

DataTypeNumber::DataTypeNumber(std::string name, long min, long max, bool globalType) :
    DataType(name, globalType),
    min(min),
    max(max)
{
}

QWidget* DataTypeNumber::getConfigWidget(DataLogger* dataLogger, CParameter *param) {
    QSpinBox* sbox = new QSpinBox();
    int minInt = (min < numeric_limits<int>::min()) ? numeric_limits<int>::min() : min; //TODO implement subclass of QAbstractSpinBox with long int support
    int maxInt = (max > numeric_limits<int>::max()) ? numeric_limits<int>::max() : max;
    sbox->setMinimum(minInt);
    sbox->setMaximum(maxInt);
    long value = atol(param->getValue().c_str());
    sbox->setValue(value);

    if (param->getCritical()) {
        QObject::connect(sbox, SIGNAL(valueChanged(int)), dataLogger, SLOT(parameterChanged()));
    }

    return sbox;
}

std::string DataTypeNumber::getConfigData(QWidget* widget) {
    QSpinBox* sbox = dynamic_cast<QSpinBox*>(widget);
    return std::to_string(sbox->value());
}

DataTypeFloat::DataTypeFloat(std::string name, double min, double max, int decimals, bool outputAsInt, bool globalType) :
    DataType(name, globalType),
    min(min),
    max(max),
    decimals(decimals),
    outputAsInt(outputAsInt)
{
}

QWidget* DataTypeFloat::getConfigWidget(DataLogger* dataLogger, CParameter *param) {
    QDoubleSpinBox* sbox = new QDoubleSpinBox();
    sbox->setMinimum(min);
    sbox->setMaximum(max);
    sbox->setDecimals(decimals);
    sbox->setSingleStep(0.1f);
    double value;
    if (outputAsInt) {
        value = atoi(param->getValue().c_str()) / exp10(decimals);
    } else {
        value = atof(param->getValue().c_str());
    }
    sbox->setValue(value);

    if (param->getCritical()) {
        QObject::connect(sbox, SIGNAL(valueChanged(int)), dataLogger, SLOT(parameterChanged()));
    }

    return sbox;
}

std::string DataTypeFloat::getConfigData(QWidget* widget) {
    QDoubleSpinBox* sbox = dynamic_cast<QDoubleSpinBox*>(widget);
    if (outputAsInt) {
        return std::to_string((int)(sbox->value() * exp10(decimals)));
    } else {
        return std::to_string(sbox->value());
    }
}

DataTypeEnumeration::DataTypeEnumeration(std::string name, bool globalType) :
    DataType(name, globalType)
{
}

DataTypeEnumeration::DataTypeEnumeration(std::string name, string headerFile, bool globalType) :
    DataType(name, headerFile, globalType)
{
}

void DataTypeEnumeration::addValue(std::string value, std::string description) {
    enumVal val = {value, description, "", 0};
    values.push_back(val);
    if (hasSuffix("_cpt")) {
        controlParameterType.values.push_back(val);
    }
}

void DataTypeEnumeration::addValues(std::list<enumVal> valueList) {
    values.insert(values.end(), valueList.begin(), valueList.end());
    if (hasSuffix("_cpt")) {
        controlParameterType.values.insert(controlParameterType.values.end(), valueList.begin(), valueList.end());
    }
}

QWidget* DataTypeEnumeration::getConfigWidget(DataLogger* dataLogger, CParameter *param) {
    return getConfigBox(dataLogger, param);
}

string DataTypeEnumeration::getEnumValueFromInt(int val) {
    for (list<enumVal>::iterator i = values.begin(); i != values.end(); i++) {
        if (i->intVal == val) {
            return i->value;
        }
    }
    return "";
}

QComboBox* DataTypeEnumeration::getConfigBox(DataLogger* dataLogger, CParameter* param) {
    QComboBox* cbox = new QComboBox();
    int pos = 0;
    for (list<enumVal>::iterator i = values.begin(); i != values.end(); i++) {
        cbox->addItem(QString((*i).value.c_str()));
        cbox->setItemData(pos, (*i).description.c_str(), Qt::ToolTipRole);
        pos++;
    }
    pos = cbox->findText(QString(param->getValue().c_str()));
    if (pos == -1) {
        int val;
        istringstream(param->getValue()) >> val;
        pos = cbox->findText(QString(getEnumValueFromInt(val).c_str()));
    }
    cbox->setCurrentIndex(pos);

    if (param->getCritical()) {
        QObject::connect(cbox, SIGNAL(currentIndexChanged(int)), dataLogger, SLOT(parameterChanged()));
    }

    return cbox;
}

std::string DataTypeEnumeration::getConfigData(QWidget* widget) {
    QComboBox* cbox = dynamic_cast<QComboBox*>(widget);
    return cbox->currentText().toStdString();
}

std::string DataTypeEnumeration::getValueReference(std::string enumerationValue) {
    for (list<enumVal>::iterator i = values.begin(); i != values.end(); i++) {
        if ((*i).value.compare(enumerationValue) == 0) {
            return (*i).valueReference;
        }
    }
    return "";
}

QWidget* DataTypeString::getConfigWidget(DataLogger*, CParameter *param) {
    QLineEdit* lEdit = new QLineEdit();
    lEdit->setText(param->getValue().c_str());
    return lEdit;
}

std::string DataTypeString::getConfigData(QWidget* widget) {
    QLineEdit* lEdit = dynamic_cast<QLineEdit*>(widget);
    return lEdit->text().toStdString();
}

QWidget* DataTypeChar::getConfigWidget(DataLogger*, CParameter *param) {
    QLineEdit* lEdit = new QLineEdit();
    lEdit->setText(param->getValue().c_str());
    lEdit->setMaxLength(1);
    return lEdit;
}

std::string DataTypeChar::getConfigData(QWidget* widget) {
    QLineEdit* lEdit = dynamic_cast<QLineEdit*>(widget);
    string res = lEdit->text().toStdString();
    if (res.empty())
        return " ";
    else
        return res;
}

DataTypePin::DataTypePin(std::string name, bool globalType) :
    DataType(name, globalType)
{

}

void DataTypePin::addPin(std::string groupName, Pin& pin) {
    pins[groupName].push_back(pin);
}

QWidget* DataTypePin::getConfigWidget(DataLogger* dataLogger, CParameter *param) {
    return getConfigWidget(dataLogger, param, NULL, NULL);
}

QWidget* DataTypePin::getConfigWidget(DataLogger* dataLogger, CParameter* param, QObject* receiver, const char* slot) {
    string pin = param->getValue();
    string groupName = Pin::getGroupFromFullName(pin);

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

    PinBox* pinBox = new PinBox(dataLogger, pin);
    layout->addWidget(pinBox);
    pinBox->setPinItems(QString(groupName.c_str()));
    pinBox->setCurrentIndex(pinBox->findText(QString(Pin::getPinFromFullName(pin).c_str())));

    QObject::connect(groupBox, SIGNAL(currentIndexChanged(QString)), pinBox, SLOT(setPinItems(QString)));

    if (receiver != NULL) {
        QObject::connect(pinBox, SIGNAL(pinChanged()), receiver, slot);
    }
    QObject::connect(pinBox, SIGNAL(pinChanged()), dataLogger, SLOT(parameterChanged()));

    return widget;
}

std::string DataTypePin::getConfigData(QWidget* widget) {
    QComboBox* groupBox = dynamic_cast<QComboBox*>(widget->children().at(1));
    QComboBox* pinBox = dynamic_cast<QComboBox*>(widget->children().at(2));

    string group = groupBox->currentText().toStdString();
    string pin = pinBox->currentText().toStdString();

    if (group.empty() || pin.empty())
        return "";

    return group + ":" + pin;
}

Pin* DataTypePin::getPin(std::string group, std::string pin) {
    try {
        list<Pin>& pinsInGroup = pins.at(group);
        for (list<Pin>::iterator i = pinsInGroup.begin(); i != pinsInGroup.end(); i++) {
            if ((*i).getName().compare(pin) == 0)
                return &*i;
        }
    } catch (exception) {

    }

    return NULL;
}

std::list<std::string> DataTypePin::getGroups() {
    std::list<std::string> keyList;
    for (map<string, list<Pin> >::iterator i = pins.begin(); i != pins.end(); i++) {
        keyList.push_back(i->first);
    }
    return keyList;
}

string Pin::getGroupFromFullName(string fullName) {
    size_t d = fullName.find(":");
    if (d < fullName.length()) {
        fullName.erase(d, fullName.length());
    }

    return fullName;
}

string Pin::getPinFromFullName(string fullName) {
    size_t d = fullName.find(":");
    if (d < fullName.length()) {
        fullName.erase(0, d + 1);
    }

    return fullName;
}

DataTypeFunction::DataTypeFunction(std::string name, bool globalType) :
    DataType(name, globalType)
{
    types[name] = this;
}

QWidget* DataTypeFunction::getConfigWidget(DataLogger*, CParameter* param) {
    QComboBox* cbox = new QComboBox();
    for (list<string>::iterator i = functionNames.begin(); i != functionNames.end(); i++) {
        cbox->addItem(QString((*i).c_str()));
    }
    cbox->setCurrentIndex(cbox->findText(QString(param->getValue().c_str())));
    return cbox;
}

std::string DataTypeFunction::getConfigData(QWidget* widget) {
    QComboBox* cbox = dynamic_cast<QComboBox*>(widget);
    return cbox->currentText().toStdString();
}

void DataTypeFunction::addFunction(std::string name, std::string signature, bool globalType) {
    DataTypeFunction* dtf;
    try {
        dtf = types.at(signature);
    } catch (exception) {
        dtf = new DataTypeFunction(signature, globalType);
    }
    dtf->functionNames.push_back(name);
}

DataType* DataTypeFunction::getType(std::string signature) {
    try {
        return DataType::getType(signature);
    } catch (exception) {
        return new DataTypeFunction(signature, true);
    }
}

DataTypeFunction::~DataTypeFunction() {
    for (list<string>::iterator i = functionNames.begin(); i != functionNames.end(); i++) {
        types.erase(*i);
    }
}
