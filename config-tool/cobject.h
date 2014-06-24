#ifndef COBJECT_H
#define COBJECT_H

#include <QObject>
#include <string>
#include "datatype.h"

class CObject : public QObject
{
    Q_OBJECT

public:
    CObject(std::string name, DataType* dataType);

    std::string getName() { return name; }
    DataType* getType() { return type; }
protected:
    std::string name;
    DataType* type;
};

#endif // COBJECT_H
