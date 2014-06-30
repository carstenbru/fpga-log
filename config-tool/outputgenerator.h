#ifndef OUTPUTGENERATOR_H
#define OUTPUTGENERATOR_H

#include "datalogger.h"
#include <iostream>
#include <map>
#include <set>

class OutputGenerator
{
public:
    OutputGenerator(DataLogger* dataLogger);

    void generateCSource();
private:
    void writeVariableDefinitions(std::ostream& stream);
    void writeInitFunction(std::ostream& stream);
    void writeConnectPorts(std::ostream& stream);
    void writeHeaderIncludes(std::ostream& stream);

    void writeObjectInit(std::ostream& stream, CObject* object, std::map<std::string, CObject *> &objects, std::map<std::string, bool>& initDone);

    DataLogger* dataLogger;
    std::set<std::string> usedHeaders;
};

#endif // OUTPUTGENERATOR_H
