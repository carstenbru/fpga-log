#ifndef COBJECT_H
#define COBJECT_H

#include <QObject>
#include <string>
#include <list>
#include "datatype.h"
#include "spmcperipheral.h"

class CObject : public QObject
{
    Q_OBJECT

public:
    CObject(std::string name, DataTypeStruct *dataType);
    virtual ~CObject();

    std::string getName() { return name; }
    void setName(std::string name) { this->name = name; }
    DataTypeStruct* getType() { return type; }

    CMethod* getInitMethod() { return initMethod; }
    std::list<SpmcPeripheral*> getPeripherals() { return peripherals; }
protected:
    std::string name;
    DataTypeStruct* type;

    CMethod* initMethod;
    std::list<SpmcPeripheral*> peripherals;
};

#endif // COBJECT_H
