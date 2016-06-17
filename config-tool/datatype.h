/**
 * @file datatype.h
 * @brief classes representing the used fpga-log datatypes (e.g. of module parameters)
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef DATATYPE_H
#define DATATYPE_H

#include <string>
#include <list>
#include <map>
#include <QWidget>
#include <QComboBox>
#include "cmethod.h"

#define NEW_OBJECT_STRING "<neues Objekt>"

class DataLogger;

/**
 * @brief super-class for datatypes, all other datatypes inherit from this
 */
class DataType
{
public:
    /**
     * @brief constructor to use when header file is unknown or not important, also used for general types like integer or string
     *
     * @param name the name of the type
     * @param globalType true if the type is a gloabal type for all dataloggers and not just from a custom module or spcific for one datalogger
     */
    DataType(std::string name, bool globalType);

    /**
     * @brief main constructor
     *
     * @param name the name of the type
     * @param headerFile name of the fpga-log header file containing the data type (where it was found)
     * @param globalType true if the type is a gloabal type for all dataloggers and not just from a custom module or spcific for one datalogger
     */
    DataType(std::string name, std::string headerFile, bool globalType);
    virtual ~DataType() {types.erase(name);}

    /**
     * @brief gets the name of the type
     *
     * @return the name of the type
     */
    std::string getName() { return name; }

    /**
     * @brief gets the name of the fpga-log header file containing the data type (where it was found)
     *
     * @return the name of the fpga-log header file containing the data type (where it was found)
     */
    std::string getHeaderName() { return headerFile; }

    /**
     * @brief gets the data type name without the "_t" prefix
     * @return  the data type name without the "_t" prefix
     */
    std::string getCleanedName();

    /**
     * @brief gets the name which should be shown to the user in GUI purposes
     *
     * @return the name which should be shown to the user in GUI purposes
     */
    std::string getDisplayName() { return getCleanedName(); }

    /**
     * @brief checks if the name starts by a given string
     *
     * @param prefix prefix to check
     * @return true if the name starts witht the given prefix, otherwise false
     */
    bool hasPrefix(std::string prefix) { return (name.find(prefix) == 0); }

    /**
     * @brief checks if the name ends by a given string
     * @param suffix suffix to check
     * @return  true if the name ends witht the given prefix, otherwise false
     */
    bool hasSuffix(std::string suffix);

    /**
     * @brief checks if the type is a global type for all dataloggers and not just from a custom module or spcific for one datalogger
     *
     * @return true if the type is a global type
     */
    bool isGlobal() {return globalType;}

    /**
     * @brief checks if this type is equal to another type
     *
     * @param dataType datatype to compare to
     * @return true if this type is equal to the other type
     */
    bool equals(DataType* dataType) {return name.compare(dataType->name) == 0;}

    /**
     * @brief gets an appropriate configuration widget for a parameter of this type
     *
     * @return a configuration widget for a parameter of this type
     */
    virtual QWidget* getConfigWidget(DataLogger*, CParameter*);

    /**
     * @brief gets the data entered into a configuration widget for a parameter of this type
     *
     * @return the data entered into a configuration widget for a parameter of this type
     */
    virtual std::string getConfigData(QWidget*) { return ""; }

    /**
     * @brief gets the default value of the datatype
     *
     * @return the default value of the datatype
     */
    virtual std::string getDefaultValue() { return ""; }

    /**
     * @brief returns the correct representation of a value in c-code for a value of this datatype
     *
     * @param value the value to transform to correct c-code
     * @return the correct representation of a value in c-code for a value of this datatype
     */
    virtual std::string getCValue(std::string value) { return value; }

    /**
     * @brief finds a datatype by its name
     *
     * @param name name of the type to search for
     * @return the requested datatype
     */
    static DataType* getType(std::string name) { return types.at(name); }

    /**
     * @brief gets the map of all known datatypes
     *
     * @return the map of all known datatypes
     */
    static std::map<std::string, DataType*> getTypes() { return types; }

    /**
     * @brief removes a local (not global) datatype from the list of known types
     */
    static void removeLocalTypes();
private:
    std::string name;
    std::string headerFile;

    bool globalType;

    static std::map<std::string, DataType*> types;
};

/**
 * @brief class representing a struct datatype, i.e. a "class" in fpga-log
 */
class DataTypeStruct : public DataType {
public:
    /**
     * @brief constructor
     *
     * @param name name of the datatype
     * @param headerFile name of the fpga-log header file containing the data type (where it was found)
     * @param globalType true if the type is a gloabal type for all dataloggers and not just from a custom module or spcific for one datalogger
     * @param description description of the class, e.g. to show in GUI
     */
    DataTypeStruct(std::string name, std::string headerFile, bool globalType, std::string description);
    virtual ~DataTypeStruct() {types.erase(getName());}

    /**
     * @brief gets the super type of the class, i.e. the class from which this tyoe inherits
     *
     * @return the super type of the class
     */
    DataTypeStruct* getSuperType() { return super; }

    /**
     * @brief gets all child classes of this type, i.e. all classes which inherit from this type
     *
     * @return all child classes of this type
     */
    std::list<DataTypeStruct*> getChilds() { return childs; }

    /**
     * @brief gets all methods of this type
     *
     * @return all methods of this type
     */
    std::list<CMethod*> getMethods() { return methods; }

    /**
     * @brief gets a method indentified by its name
     *
     * @param methodName the name of the method to find
     * @return the requested method of this datatype
     */
    CMethod* getMethod(std::string methodName);

    /**
     * @brief checks it this object is instantiable, i.e. not abstract
     *
     * @return true if the object is instantiable
     */
    bool isInstantiableObject();

    /**
     * @brief checks whether a type is a super class of this type, i.e. this type inherits from it (not neccissarily directly)
     *
     * @param dataType class to check for
     * @return true if the class is a superclass of this type
     */
    bool isSuperclass(DataTypeStruct* dataType);

    /**
     * @brief sets the super type of this class
     *
     * @param super the new super class
     */
    void setSuperType(DataTypeStruct* super) { this->super = super; }

    /**
     * @brief adds a child class to this type which inherits from it
     *
     * @param child the new child class
     */
    void addChild(DataTypeStruct* child) { childs.push_back(child); }

    /**
     * @brief adds a method to this type
     *
     * @param method the new method
     * @param inherit true if child classes should inherit this method
     */
    void addMethod(CMethod* method, bool inherit);

    /**
     * @brief gets the description of the type, e.g. to show in GUI
     *
     * @return the description of the type
     */
    std::string getDescription() {return description;}

    /**
     * @brief sets the weight of an object of this type
     *
     * The weight value is used by the AutomaticCoreAssigner to partition the system on multiple cores
     *
     * @param weight the new weight value
     */
    void setWeight(int weight) {this->weight = weight;}

    /**
     * @brief gets the weight of an object of this type
     *
     *  The weight value is used by the AutomaticCoreAssigner to partition the system on multiple cores
     *
     * @return the weight of an object of this type
     */
    int getWeight() {return weight;}

    virtual QWidget* getConfigWidget(DataLogger* dataLogger, CParameter* param);
    virtual std::string getConfigData(QWidget* widget);

    /**
     * @brief gets the DataTypeStruct by name
     *
     * @param name the name to search for
     * @return the requested datatype
     */
    static DataTypeStruct* getType(std::string name) { return types.at(name); }
    /**
     * @brief gets the map of all datatypes (structs, classes)
     * @return the map of all datatypes (structs, classes)
     */
    static std::map<std::string, DataTypeStruct*> getTypes() { return types; }
private:
    DataTypeStruct* super;

    std::list<DataTypeStruct*> childs;
    std::list<CMethod*> methods;

    std::string description;
    int weight;

    static std::map<std::string, DataTypeStruct*> types;
};

/**
 * @brief class representing a number (integer) datatype
 */
class DataTypeNumber : public DataType {
public:
    /**
     * @brief constructor
     *
     * @param name name of the new datatype
     * @param min minimum value
     * @param max maximum value
     * @param globalType true if the type is a gloabal type for all dataloggers and not just from a custom module or spcific for one datalogger
     */
    DataTypeNumber(std::string name, long min, long max, bool globalType);
    virtual ~DataTypeNumber() {}

    /**
     * @brief gets the minimum value representable by this type
     *
     * @return the minimum value representable by this type
     */
    long getMin() { return min; }

    /**
     * @brief gets the maximum value representable by this type
     *
     * @return the maximum value representable by this type
     */
    long getMax() { return max; }

    virtual QWidget* getConfigWidget(DataLogger*dataLogger, CParameter* param);
    virtual std::string getConfigData(QWidget* widget);
    virtual std::string getDefaultValue() { return std::to_string((min < 0) ? 0 : min); }
private:
    long min;
    long max;
};

/**
 * @brief class representing a number (float) datatype
 */
class DataTypeFloat : public DataType {
public:
    /**
     * @brief constructor
     *
     * @param name name of the new datatype
     * @param min minimum value
     * @param max maximum value
     * @param decimals number of decimal digits the type stores
     * @param outputAsInt true to store and output the value (in the code) as integer (e.g. for fixed-point types)
     * @param globalType true if the type is a gloabal type for all dataloggers and not just from a custom module or spcific for one datalogger
     */
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

/**
  * @brief stuct defining a single value in an enumeration
  */
typedef struct {
    std::string value; /**< the actual value */
    std::string description; /**< description of the value */
    std::string valueReference; /**< referenced value which could influence this value */
    int intVal; /**< integer representation of the value */
} enumVal;

/**
 * @brief enumeration datatypes class
 */
class DataTypeEnumeration : public DataType {
public:
    /**
     * @brief contructor
     *
     * @param name name of the new datatype
     * @param globalType true if the type is a gloabal type for all dataloggers and not just from a custom module or spcific for one datalogger
     */
    DataTypeEnumeration(std::string name, bool globalType);

    /**
     * @brief contructor
     *
     * @param name name of the new datatype
     * @param headerFile name of the fpga-log header file containing the data type (where it was found)
     * @param globalType true if the type is a gloabal type for all dataloggers and not just from a custom module or spcific for one datalogger
     */
    DataTypeEnumeration(std::string name, std::string headerFile, bool globalType);
    virtual ~DataTypeEnumeration() {}

    /**
     * @brief adds a value to the enumeration
     *
     * @param value the value itself
     * @param description description of the value
     */
    void addValue(std::string value, std::string description);

    /**
     * @brief adds a value to the enumeration
     *
     * @param value the value itself
     */
    void addValue(std::string value) { addValue(value, ""); }

    /**
     * @brief adds a list of values to the enumeration
     *
     * @param valueList the list of values to add
     */
    void addValues(std::list<enumVal> valueList);

    virtual QWidget* getConfigWidget(DataLogger*dataLogger, CParameter* param);
    virtual std::string getConfigData(QWidget* widget);
    virtual std::string getDefaultValue() { return values.front().value; }

    /**
     * @brief gets the enumeration value from its integer representation
     *
     * @param val the integer value
     * @return the enumeration value
     */
    std::string getEnumValueFromInt(int val);

    /**
     * @brief gets a (possible) reference from a enumeration value
     *
     * @param enumerationValue the enumeration value to look for
     * @return the name of the reference
     */
    std::string getValueReference(std::string enumerationValue);

    /**
     * @brief gets a selection box widget for the enumeration type
     *
     * @param dataLogger the parent dataLogger
     * @param param the parameter the widget should belong to
     * @return  a selection box widget for the enumeration type
     */
    QComboBox* getConfigBox(DataLogger* dataLogger, CParameter *param);
private:
    std::list<enumVal> values;

    static DataTypeEnumeration controlParameterType;
};

/**
 * @brief a class for boolean datatypes
 */
class DataTypeBoolean : public DataTypeEnumeration {
public:
    /**
     * @brief contructor
     *
     * @param name name of the new datatype
     * @param globalType true if the type is a gloabal type for all dataloggers and not just from a custom module or spcific for one datalogger
     */
    DataTypeBoolean(std::string name, bool globalType);

    /**
     * @brief contructor
     *
     * @param name name of the new datatype
     * @param headerFile name of the fpga-log header file containing the data type (where it was found)
     * @param globalType true if the type is a gloabal type for all dataloggers and not just from a custom module or spcific for one datalogger
     */
    DataTypeBoolean(std::string name, std::string headerFile, bool globalType);
    virtual ~DataTypeBoolean() {}

    virtual QWidget* getConfigWidget(DataLogger*dataLogger, CParameter* param);
    virtual std::string getConfigData(QWidget* widget);
};

/**
 * @brief a class for string datatypes
 */
class DataTypeString : public DataType {
public:
    /**
     * @brief contructor
     *
     * @param name name of the new datatype
     * @param globalType true if the type is a gloabal type for all dataloggers and not just from a custom module or spcific for one datalogger
     */
    DataTypeString(std::string name, bool globalType) : DataType(name, globalType) {}
    virtual ~DataTypeString() {}

    virtual QWidget* getConfigWidget(DataLogger*, CParameter* param);
    virtual std::string getConfigData(QWidget* widget);

    virtual std::string getCValue(std::string value) { return "\"" + value + "\""; }
};

/**
 * @brief a class for character datatypes
 */
class DataTypeChar : public DataType {
public:
    /**
     * @brief contructor
     *
     * @param name name of the new datatype
     * @param globalType true if the type is a gloabal type for all dataloggers and not just from a custom module or spcific for one datalogger
     */
    DataTypeChar(std::string name, bool globalType) : DataType(name, globalType) {}
    virtual ~DataTypeChar() {}

    virtual QWidget* getConfigWidget(DataLogger*, CParameter* param);
    virtual std::string getConfigData(QWidget* widget);
    virtual std::string getDefaultValue() { return " "; }

    virtual std::string getCValue(std::string value) { return "'" + value + "'"; }
};

/**
 * @brief class representing a pin of the datalogger
 */
class Pin {
public:
    /**
     * @brief constructor
     *
     * @param name name of the pin
     */
    Pin(std::string name) : name(name) {}
    ~Pin() {}

    /**
     * @brief gets the name of the pin
     *
     * @return the name of the pin
     */
    std::string getName() { return name; }

    /**
     * @brief gets a possible frequency constraint of a pin
     *
     * @return a frequency constraint of a pin
     */
    std::string getFreq() { return freq; }

    /**
     * @brief gets the location on the FPGA of the pin
     *
     * @return the location on the FPGA of the pin
     */
    std::string getLoc() { return loc; }

    /**
     * @brief sets the frequency constraint of a pin
     *
     * @param freq the new frequency constraint
     */
    void setFreq(std::string freq) { this->freq = freq; }

    /**
     * @brief sets the location on the FPGA of the pin
     * @param loc the new location on the FPGA of the pin
     */
    void setLoc(std::string loc) { this->loc = loc; }

    /**
     * @brief gets the pins group from its full name
     *
     * @param fullName the full name of the pin
     * @return the pins group from its full name
     */
    static std::string getGroupFromFullName(std::string fullName);

    /**
     * @brief gets the pin name from its full name
     * @param fullName the full name of the pin
     * @return the pin name
     */
    static std::string getPinFromFullName(std::string fullName);
private:
    std::string name;

    std::string freq;
    std::string loc;
};

/**
 * @brief datatype representing a pin
 *
 * There is on static object of this class holding all pins in the current design.
 */
class DataTypePin : public DataType {
public:
    /**
     * @brief contructor
     *
     * @param name name of the new datatype
     * @param globalType true if the type is a gloabal type for all dataloggers and not just from a custom module or spcific for one datalogger
     */
    DataTypePin(std::string name, bool globalType);
    virtual ~DataTypePin() {}

    virtual QWidget* getConfigWidget(DataLogger*dataLogger, CParameter* param);
    /**
     * @brief gets an appropriate configuration widget for a parameter of this type with a receiver for value changes
     *
     * @param dataLogger the dataLogger where we belong to
     * @param param the parameter the configuration widget is for
     * @param receiver signal receiver for selection changes
     * @param slot receive slot for selection changes
     * @return a configuration widget for a parameter of this type
     */
    QWidget* getConfigWidget(DataLogger* dataLogger, CParameter* param, QObject* receiver, const char *slot);
    virtual std::string getConfigData(QWidget* widget);

    /**
     * @brief adds a new pin to the pin type
     *
     * @param groupName group name of the pin
     * @param pin name of the pin
     */
    void addPin(std::string groupName, Pin &pin);

    /**
     * @brief gets a list of all pin groups
     *
     * @return a list of all pin groups
     */
    std::list<std::string> getGroups();

    /**
     * @brief gets a list of all pins in a particular group
     *
     * @param group the group to search for
     * @return a list of all pins in a particular group
     */
    std::list<Pin> getPinsInGroup(std::string group) { return pins.at(group); }

    /**
     * @brief gets a pins class
     *
     * @param group group of the pin
     * @param pin name of the pin
     * @return the pins class
     */
    Pin* getPin(std::string group, std::string pin);

    /**
     * @brief clears the pin datatype, i.e. removing all know pins
     */
    void clear() { pins.clear(); }

    /**
     * @brief gets the (static) pin datatype
     *
     * @return the (static) pin datatype
     */
    static DataTypePin* getPinType() { return &pinType; }
private:
    static DataTypePin pinType;
    std::map<std::string, std::list<Pin> > pins;
};

/**
 * @brief datatype representing a function
 */
class DataTypeFunction : public DataType {
public:
    /**
     * @brief contructor
     *
     * @param name name of the new datatype
     * @param globalType true if the type is a gloabal type for all dataloggers and not just from a custom module or spcific for one datalogger
     */
    DataTypeFunction(std::string name, bool globalType);
    virtual ~DataTypeFunction();

    virtual QWidget* getConfigWidget(DataLogger*, CParameter* param);
    virtual std::string getConfigData(QWidget* widget);

    /**
     * @brief adds a function to the list of known functions
     *
     * @param name name of the new function
     * @param signature signature of the function (i.e. type of parameters and return type)
     * @param globalType true if the function should be globally available, false if only in the current system
     */
    static void addFunction(std::string name, std::string signature, bool globalType);

    /**
     * @brief gets a function datatype fitting a signature definition
     *
     * @param signature the signature defintion (i.e. type of parameters and return type)
     * @return the function datatype with the requested signature
     */
    static DataType* getType(std::string signature);
private:
    std::list<std::string> functionNames;

    static std::map<std::string, DataTypeFunction*> types;
};

#endif // DATATYPE_H
