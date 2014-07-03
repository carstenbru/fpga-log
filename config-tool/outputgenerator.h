#ifndef OUTPUTGENERATOR_H
#define OUTPUTGENERATOR_H

#include "datalogger.h"
#include <iostream>
#include <map>
#include <set>

class FpgaPin {
public:
    FpgaPin(std::string name, std::string direction) : name(name), direction(direction) {}

    std::string getName() { return name; }
    std::string getDirection() { return direction; }
private:
    std::string name;
    std::string direction;
};

class OutputGenerator
{
public:
    OutputGenerator(DataLogger* dataLogger);

    void generateCSource();
    void generateSystemXML();
private:
    void writeVariableDefinitions(std::ostream& stream);
    void writeInitFunction(std::ostream& stream);
    void writeConnectPorts(std::ostream& stream);
    void writeAdvancedConfig(std::ostream& stream);
    void writePreamble(std::ostream& stream);
    void writeHeaderIncludes(std::ostream& stream);

    void writeObjectInit(std::ostream& stream, CObject* object, std::map<std::string, CObject *> &objects, std::map<std::string, bool>& initDone);

    void writeAttributeElement(QXmlStreamWriter& writer, QString id, QString text);
    void writeTargetNode(QXmlStreamWriter& writer);

    void writeConnection(QXmlStreamWriter& writer, std::string destination, int lsb);
    void writePortConnection(QXmlStreamWriter& writer, std::string port, std::string destination, int lsb);
    void writeParameter(QXmlStreamWriter& writer, CParameter* parameter);
    void writePeripheral(QXmlStreamWriter& writer, SpmcPeripheral* peripheral);
    void writePeripherals(QXmlStreamWriter& writer);
    void writeSpmcConnections(QXmlStreamWriter& writer);

    void writeClkPin(QXmlStreamWriter& writer);
    void writePins(QXmlStreamWriter& writer);

    DataLogger* dataLogger;
    std::set<std::string> usedHeaders;

    int usedIdCounter;
    std::list<FpgaPin> usedPins;
};

#endif // OUTPUTGENERATOR_H
