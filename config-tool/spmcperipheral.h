#ifndef SPMCPERIPHERAL_H
#define SPMCPERIPHERAL_H

#include <list>
#include <string>
#include <map>
#include "datatype.h"
#include "cparameter.h"

class SpmcPeripheral
{
public:
    SpmcPeripheral(DataType* dataType);
    ~SpmcPeripheral();

    std::list<CParameter*> getParameters() { return parameters; }

    static void loadPeripheralXMLs();
private:
    std::string getFileName();
    void readParametersFromFile();

    static std::string readModuleNameFromFile(std::string fileName);

    DataType* dataType;
    std::list<CParameter*> parameters;

    static std::map<std::string, std::string> peripheralXMLs;
};

#endif // SPMCPERIPHERAL_H
