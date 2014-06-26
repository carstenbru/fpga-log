#ifndef SPMCPERIPHERAL_H
#define SPMCPERIPHERAL_H

#include <list>
#include <string>
#include "datatype.h"
#include "cparameter.h"

class SpmcPeripheral
{
public:
    SpmcPeripheral(DataType* dataType);
    ~SpmcPeripheral();

    std::list<CParameter*> getParameters() { return parameters; }
private:
    std::string getFileName();
    void readParametersFromFile();

    DataType* dataType;
    std::list<CParameter*> parameters;
};

#endif // SPMCPERIPHERAL_H
