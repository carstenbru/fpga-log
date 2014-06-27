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
    SpmcPeripheral(DataType* dataType, std::string parentModuleName, DataLogger *dataLogger);
    ~SpmcPeripheral();

    std::list<CParameter*> getParameters() { return parameters; }
    CParameter* getParameter(std::string name);

    static void loadPeripheralXMLs();
private:
    std::string getFileName();
    void readParametersFromFile();
    void readModuleXML();

    static std::string readModuleNameFromFile(std::string fileName);

    DataLogger* dataLogger;

    DataType* dataType;
    std::list<CParameter*> parameters;

    std::string parentModuleName;

    static std::map<std::string, std::string> peripheralXMLs;
};

#endif // SPMCPERIPHERAL_H
