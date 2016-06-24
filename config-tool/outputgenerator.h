/**
 * @file outputgenerator.h
 * @brief OutputGenerator class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef OUTPUTGENERATOR_H
#define OUTPUTGENERATOR_H

#include <iostream>
#include <map>
#include <set>
#include <QObject>
#include <QProcess>
#include <sstream>
#include "cobject.h"

//defines the maximum number of block RAMs used, this is set in a first compile run and then for the second run
//the block RAM counts are set to the real needed values
#define MAX_BLOCK_RAMS 42
//true to optimize the number of block RAMs in a second compile run, otherwise a fixed number of MAX_BLOCK_RAMS is used for every core
#define OPTIMIZE_BLOCK_RAMS true

//DataLogger forward declariation,including the header here would lead to a cycle
class DataLogger;

/**
 * @brief class representing a physical FPGA pin with its properties
 */
class FpgaPin {
public:
    /**
     * @brief constructor
     *
     * @param name name of the pin, should be the physical pin name of the target board
     * @param direction direction of the pin
     * @param constraints other constraints for the pin (e.g. PULLUP definitions)
     */
    FpgaPin(std::string name, std::string direction, std::string constraints) : name(name), direction(direction), constraints(constraints) {}

    /**
     * @brief gets the name of the pin
     *
     * @return the name of the pin
     */
    std::string getName() { return name; }

    /**
     * @brief gets the direction of the pin
     * @return the direction of the pin
     */
    std::string getDirection() { return direction; }

    /**
     * @brief gets the constraints defined for the pin
     *
     * @return the constraints defined for the pin
     */
    std::string getConstraints() { return constraints; }
private:
    std::string name;
    std::string direction;

    std::string constraints;
};

/**
 * @brief class for generating the datalogger
 *
 * This class is responsible for writing source files (C-source and SpartanMC system XML definition) and invoking external synthesis commands.
 */
class OutputGenerator : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief constructor
     *
     * @param dataLogger datalogger for which files should be written or commands executed
     * @param directory working directory
     * @param changeOtherModulesCoreID true to automatically infer core assignments for non datastream objects
     */
    OutputGenerator(DataLogger* dataLogger, std::string directory, bool changeOtherModulesCoreID = true);
    ~OutputGenerator();

    /**
     * @brief generates all configuration files for the datalogger (c-source and SpartanMC system XML definition)
     */
    void generateConfigFiles();

    /**
     * @brief generates the configuration files and then starts FPGA synthesis
     */
    void synthesizeSystem();

    /**
     * @brief only starts FPGA synthesis without re-generating the configuration files before
     */
    void synthesizeOnly();

    /**
     * @brief flashes the generated system to the target
     */
    void flash();

    /**
     * @brief measures the resulting firmware size for a system
     * @see firmwareSizeResult
     */
    void measureFirmwareSize();

    /**
     * @brief stops the currently executed task
     */
    void stopTasks();
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
    void writePeripheral(QXmlStreamWriter& writer, SpmcPeripheral* peripheral, std::string subsystemID, int subsystemNumber);
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
    void determineTimestampCounterPeripheralCore();

    DataLogger* dataLogger;

    std::string directory;

    std::set<std::string> usedHeaders;

    int usedIdCounter;
    std::list<FpgaPin> usedPins;
    std::map<int, int> usedTimestampSources;
    std::map<int, int> usedTimestampPinSources;
    int timestampCounterPeripheralCore;

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
    /**
     * @brief emitted when the current task is finished completly
     *
     * @param errorOccured true if an error occured during task execution
     * @param timingError true if a timing error occured during task execution
     */
    void finished(bool errorOccured, bool timingError);

    /**
     * @brief emitted when an error is found during task execution
     *
     * @param message a message describing the detected error
     */
    void errorFound(std::string message);

    /**
     * @brief emitted after the firmware size calculation of a system is finished
     * @see measureFirmwareSize
     *
     * @param size measured firmware size
     */
    void firmwareSizeResult(int size);
};

#endif // OUTPUTGENERATOR_H
