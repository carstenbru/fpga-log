#ifndef COBJECT_H
#define COBJECT_H

#include <QObject>
#include <string>
#include <map>
#include "datatype.h"

class CObject : public QObject
{
    Q_OBJECT

public:
    CObject(std::string name, DataTypeStruct *dataType);

    std::string getName() { return name; }
    void setName(std::string name) { this->name = name; }
    DataTypeStruct* getType() { return type; }

    void setReqParameter(std::string parameter, std::string value) { reqParameters[parameter] = value; }
    std::string getReqParameter(std::string parameter) { return reqParameters[parameter]; }
protected:
    std::string name;
    DataTypeStruct* type;

    std::map<std::string, std::string> reqParameters;
};

#endif // COBJECT_H
