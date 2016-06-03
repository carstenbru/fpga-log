#ifndef OUTPUTGENERATOR_H
#define OUTPUTGENERATOR_H

#include <iostream>
#include <map>
#include <set>
#include <QObject>
#include <QProcess>
#include <sstream>
#include "cobject.h"

#define MAX_BLOCK_RAMS 42
#define OPTIMIZE_BLOCK_RAMS true

class DataLogger;

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
    OutputGenerator(DataLogger* dataLogger, std::string directory, bool changeOtherModulesCodeID = true);
    ~OutputGenerator();

    void generateConfigFiles();
    void synthesizeSystem();
    void synthesizeOnly();
    void flash();
    void measureFirmwareSize();
private:
    void copyProjectTemplate();
    void copyMainC(QString src, QString dest);

    void generateCSources();
    void generateCSource(int subsystemID);
    void generateSystemXML(bool useMaxBlockRAMs);
    void generateSpmcSubsystem(std::ostream &stream, int id, bool useMaxBlockRAMs);

    void determineHeaders();

    void writeVariableDefinitions(std::ostream& stream, int subsystemID);
    void writeInitFunction(std::ostream& stream, int subsystemID);
    void writeConnectPorts(std::ostream& stream, int subsystemID);
    void writeAdvancedConfig(std::ostream& stream, int subsystemID);
    void writePreamble(std::ostream& stream);
    void writeHeaderIncludes(std::ostream& stream);

    void putUsedHeader(std::string headerName, bool global);

    void writeObjectInit(std::ostream& stream, CObject* object, std::map<std::string, CObject *> &objects, std::map<std::string, bool>& initDone);
    void writeMethod(std::ostream& stream, CObject *object, CMethod* method, std::map<std::string, CObject *> &objects);

    void writeAttributeElement(QXmlStreamWriter& writer, QString id, QString text);
    void writeTargetNode(QXmlStreamWriter& writer);

    void writeConnection(QXmlStreamWriter& writer, std::string destination, int lsb);
    void writePortConnection(QXmlStreamWriter& writer, std::string port, std::string destination, int lsb);
    void writeParameter(QXmlStreamWriter& writer, CParameter* parameter);
    void writePeripheral(QXmlStreamWriter& writer, SpmcPeripheral* peripheral, std::__cxx11::string subsystemID, int subsystemNumber);
    void writeTimestampGen(QXmlStreamWriter& writer, std::string subsystemID, int subsystemNumber);
    void writePeripherals(QXmlStreamWriter& writer, std::string subsystemID, int subsystemNumber);
    void writeSpmcConnections(QXmlStreamWriter& writer, std::string subsystemID);
    void writeDMAConnections(QXmlStreamWriter& writer, std::string subsystemID);
    void addTimestampPinConnections(PeripheralPort *timestampPinPort, int id);
    void writeClkConnection(std::ostream& stream, std::string destination);

    void writeClkPin(QXmlStreamWriter& writer);
    void writePins(QXmlStreamWriter& writer);

    void calculateUsedProcessorIDs();

    void definePeripheralForSimulation(std::string name, DataType* dataType);

    void exec(std::string cmd);

    void checkSynthesisMessage(std::string message);
    void checkFirmwareSizeMessage(std::string message);

    std::string readLastLine(std::string fileName);
    int determineMinBlockRAMcount(int cpuID);

    DataLogger* dataLogger;

    std::string directory;

    std::set<std::string> usedHeaders;

    int usedIdCounter;
    std::list<FpgaPin> usedPins;
    std::map<int, int> usedTimestampSources;
    std::map<int, int> usedTimestampPinSources;

    QProcess process;
    std::list<std::string> pending;
    bool busy;

    bool error;

    int pcPeripheralIdCounter;
    std::stringstream pcPeripheralsStream;
    std::map<int,std::stringstream> pcTimestampCaptureStream;
    int pcPeripheralCompareCounter;
    int pcPeripheralTimerCounter;

    long timestampInvertMask;

    bool timingError;
    bool synthesisSuccessful;

    std::set<int> processorSet;

    bool optimizeBlockRAMcount;

    bool firmwareSizeMeasurement;
private slots:
    void newChildStdOut();
    void newChildErrOut();
    void processFinished();
signals:
    void finished(bool errorOccured, bool timingError);
    void errorFound(std::string message);
    void firmwareSizeResult(int size);
};

#endif // OUTPUTGENERATOR_H
