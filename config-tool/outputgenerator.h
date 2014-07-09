#ifndef OUTPUTGENERATOR_H
#define OUTPUTGENERATOR_H

#include "datalogger.h"
#include <iostream>
#include <map>
#include <set>
#include <QObject>
#include <QProcess>

class FpgaPin {
public:
    FpgaPin(std::string name, std::string direction, std::string constraints) : name(name), direction(direction), constraints(constraints) {}

    std::string getName() { return name; }
    std::string getDirection() { return direction; }

    std::string getConstraints() { return constraints; }
private:
    std::string name;
    std::string direction;

    std::string constraints;
};

class OutputGenerator : public QObject
{
    Q_OBJECT

public:
    OutputGenerator(DataLogger* dataLogger, std::string directory);

    void generateConfigFiles();
    void synthesizeSystem();
    void flash();
private:
    void copyProjectTemplate();

    void generateCSource();
    void generateSystemXML();

    void writeVariableDefinitions(std::ostream& stream);
    void writeInitFunction(std::ostream& stream);
    void writeConnectPorts(std::ostream& stream);
    void writeAdvancedConfig(std::ostream& stream);
    void writePreamble(std::ostream& stream);
    void writeHeaderIncludes(std::ostream& stream);

    void writeObjectInit(std::ostream& stream, CObject* object, std::map<std::string, CObject *> &objects, std::map<std::string, bool>& initDone);
    void writeMethod(std::ostream& stream, CObject *object, CMethod* method, std::map<std::string, CObject *> &objects);

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

    void exec(std::string cmd);

    DataLogger* dataLogger;

    std::string directory;

    std::set<std::string> usedHeaders;

    int usedIdCounter;
    std::list<FpgaPin> usedPins;
    int usedTimestampSources;

    QProcess process;
    std::list<std::string> pending;
    bool busy;

    bool error;
private slots:
    void newChildStdOut();
    void newChildErrOut();
    void processFinished();
signals:
    void finished(bool errorOccured);
};

#endif // OUTPUTGENERATOR_H
