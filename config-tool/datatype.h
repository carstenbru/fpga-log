#ifndef DATATYPE_H
#define DATATYPE_H

#include <string>
#include <list>
#include <map>
#include <QWidget>
#include <QComboBox>
#include "cmethod.h"

class DataLogger;

class DataType
{
public:
    DataType(std::string name, bool globalType);
    DataType(std::string name, std::string headerFile, bool globalType);
    virtual ~DataType() {types.erase(name);}

    std::string getName() { return name; }
    std::string getHeaderName() { return headerFile; }
    std::string getCleanedName();
    std::string getDisplayName() { return getCleanedName(); }
    bool hasPrefix(std::string prefix) { return (name.find(prefix) == 0); }
    bool hasSuffix(std::string suffix);
    bool isGlobal() {return globalType;}

    virtual QWidget* getConfigWidget(DataLogger*, CParameter*);
    virtual std::string getConfigData(QWidget*) { return ""; }
    virtual std::string getDefaultValue() { return ""; }

    virtual std::string getCValue(std::string value) { return value; }

    static DataType* getType(std::string name) { return types.at(name); }
    static std::map<std::string, DataType*> getTypes() { return types; }
    static void removeLocalTypes();
private:
    std::string name;
    std::string headerFile;

    bool globalType;

    static std::map<std::string, DataType*> types;
};

class DataTypeStruct : public DataType {
public:
    DataTypeStruct(std::string name, std::string headerFile, bool globalType);
    virtual ~DataTypeStruct() {types.erase(getName());}

    DataTypeStruct* getSuperType() { return super; }
    std::list<DataTypeStruct*> getChilds() { return childs; }
    std::list<CMethod*> getMethods() { return methods; }
    CMethod* getMethod(std::string methodName);
    bool isInstantiableObject();
    bool isSuperclass(DataTypeStruct* dataType);

    void setSuperType(DataTypeStruct* super) { this->super = super; }
    void addChild(DataTypeStruct* child) { childs.push_back(child); }
    void addMethod(CMethod* method, bool inherit);

    virtual QWidget* getConfigWidget(DataLogger* dataLogger, CParameter* param);
    virtual std::string getConfigData(QWidget* widget);

    static DataTypeStruct* getType(std::string name) { return types.at(name); }
    static std::map<std::string, DataTypeStruct*> getTypes() { return types; }
private:
    DataTypeStruct* super;

    std::list<DataTypeStruct*> childs;
    std::list<CMethod*> methods;

    static std::map<std::string, DataTypeStruct*> types;
};

class DataTypeNumber : public DataType {
public:
    DataTypeNumber(std::string name, long min, long max, bool globalType);
    virtual ~DataTypeNumber() {}

    virtual QWidget* getConfigWidget(DataLogger*dataLogger, CParameter* param);
    virtual std::string getConfigData(QWidget* widget);
    virtual std::string getDefaultValue() { return std::to_string((min < 0) ? 0 : min); }
private:
    long min;
    long max;
};

class DataTypeFloat : public DataType {
public:
    DataTypeFloat(std::string name, double min, double max, int decimals, bool outputAsInt, bool globalType);
    virtual ~DataTypeFloat() {}

    virtual QWidget* getConfigWidget(DataLogger*dataLogger, CParameter* param);
    virtual std::string getConfigData(QWidget* widget);
    virtual std::string getDefaultValue() { return std::to_string((min < 0) ? 0 : min); }
private:
    double min;
    double max;
    int decimals;
    bool outputAsInt;
};

class DataTypeEnumeration : public DataType {
public:
    DataTypeEnumeration(std::string name, bool globalType);
    DataTypeEnumeration(std::string name, std::string headerFile, bool globalType);
    virtual ~DataTypeEnumeration() {}

    void addValue(std::string value);
    void addValues(std::list<std::string> valueList);

    virtual QWidget* getConfigWidget(DataLogger*, CParameter* param);
    virtual std::string getConfigData(QWidget* widget);
    virtual std::string getDefaultValue() { return values.front(); }

    QComboBox* getConfigBox(CParameter *param);
private:
    std::list<std::string> values;

    static DataTypeEnumeration controlParameterType;
};

class DataTypeString : public DataType {
public:
    DataTypeString(std::string name, bool globalType) : DataType(name, globalType) {}
    virtual ~DataTypeString() {}

    virtual QWidget* getConfigWidget(DataLogger*, CParameter* param);
    virtual std::string getConfigData(QWidget* widget);

    virtual std::string getCValue(std::string value) { return "\"" + value + "\""; }
};

class DataTypeChar : public DataType {
public:
    DataTypeChar(std::string name, bool globalType) : DataType(name, globalType) {}
    virtual ~DataTypeChar() {}

    virtual QWidget* getConfigWidget(DataLogger*, CParameter* param);
    virtual std::string getConfigData(QWidget* widget);
    virtual std::string getDefaultValue() { return " "; }

    virtual std::string getCValue(std::string value) { return "'" + value + "'"; }
};

class Pin {
public:
    Pin(std::string name) : name(name) {}
    ~Pin() {}

    std::string getName() { return name; }
    std::string getFreq() { return freq; }
    void setFreq(std::string freq) { this->freq = freq; }

    static std::string getGroupFromFullName(std::string fullName);
    static std::string getPinFromFullName(std::string fullName);
private:
    std::string name;

    std::string freq;
};

class DataTypePin : public DataType {
public:
    DataTypePin(std::string name, bool globalType);
    virtual ~DataTypePin() {}

    virtual QWidget* getConfigWidget(DataLogger*dataLogger, CParameter* param);
    QWidget* getConfigWidget(DataLogger*, CParameter* param, QObject* receiver, const char *slot);
    virtual std::string getConfigData(QWidget* widget);

    void addPin(std::string groupName, Pin &pin);
    std::list<Pin> getPinsInGroup(std::string group) { return pins.at(group); }
    Pin* getPin(std::string group, std::string pin);
    void clear() { pins.clear(); }

    static DataTypePin* getPinType() { return &pinType; }
private:
    static DataTypePin pinType;
    std::map<std::string, std::list<Pin> > pins;
};

class DataTypeFunction : public DataType {
public:
    DataTypeFunction(std::string name, bool globalType);
    virtual ~DataTypeFunction();

    virtual QWidget* getConfigWidget(DataLogger*, CParameter* param);
    virtual std::string getConfigData(QWidget* widget);

    static void addFunction(std::string name, std::string signature, bool globalType);
    static DataType* getType(std::string signature);
private:
    std::list<std::string> functionNames;

    static std::map<std::string, DataTypeFunction*> types;
};

#endif // DATATYPE_H
