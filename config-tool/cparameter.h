#ifndef CPARAMETER_H
#define CPARAMETER_H

#include <string>
#include <map>

class DataType;

class CParameter {
public:
    CParameter(std::string name, DataType* dataType, bool pointer);
    CParameter(std::string name, DataType* dataType, bool pointer, std::string value);

    bool sameSignature(CParameter& compare);
    std::string getName() { return name; }
    DataType* getDataType() { return dataType; }

    void setHideFromUser(bool hide) { hideFromUser = hide; }
    bool getHideFromUser() { return hideFromUser; }

    void setValue(std::string value) { this->value = value; }
    std::string getValue() { return value; }
private:
    std::string name;
    std::string value;

    bool hideFromUser;

    DataType* dataType;
    bool pointer;
};

#endif // CPARAMETER_H
