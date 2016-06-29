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
    /**
     * @brief copies all files from the project template to the projects working directory
     */
    void copyProjectTemplate();
    /**
     * @brief copies the main.c source file with setting the correct CLOCK_FREQUENCY define in it
     *
     * @param src source file
     * @param dest destination file
     */
    void copyMainC(QString src, QString dest);

    /**
     * @brief generates the source code files for all SpartanMC systems in the logger
     */
    void generateCSources();
    /**
     * @brief generates the source code files for one SpartanMC system in the logger
     *
     * @param subsystemID ID of the system for which the source is generated
     */
    void generateCSource(int subsystemID);
    /**
     * @brief generates the system.xml system description file used by the SpartanMC toolkit (read in by JConfig)
     *
     * useMaxBlockRAMs can only be false in the second run as the output of the first run is needed to determine the BlockRAM count.
     * Therefore in the first run this parameter has to be true.
     *
     * @param useMaxBlockRAMs if true the maximum number of RAM blocks defined will be used, if false the actual number will be determined
     */
    void generateSystemXML(bool useMaxBlockRAMs);
    /**
     * @brief generates a SpartanMC subsystem (one core of the datalogger) in the system description used by the SpartanMC toolkit
     *
     * @param stream target XML output stream to write to
     * @param id ID of the subsystem to generate
     * @param useMaxBlockRAMs if true the maximum number of RAM blocks defined will be used, if false the actual number will be determined
     */
    void generateSpmcSubsystem(std::ostream &stream, int id, bool useMaxBlockRAMs);

    /**
     * @brief determines the header files used by the source code, so which files have to be included
     */
    void determineHeaders();

    /**
     * @brief writes the global variable definitions for all objects of a subsystem
     *
     * @param stream destination stream
     * @param subsystemID ID of the subsystem for which the definitions should be written
     */
    void writeVariableDefinitions(std::ostream& stream, int subsystemID);
    /**
     * @brief writes the init function (init_objects(void))
     *
     * @param stream destination stream
     * @param subsystemID ID of the subsystem
     */
    void writeInitFunction(std::ostream& stream, int subsystemID);
    /**
     * @brief writes the connect ports function (connect_ports(void))
     *
     * @param stream destination stream
     * @param subsystemID ID of the subsystem
     */
    void writeConnectPorts(std::ostream& stream, int subsystemID);
    /**
     * @brief writes the advanced configuration function (advanced_config(void))
     *
     * @param stream destination stream
     * @param subsystemID ID of the subsystem
     */
    void writeAdvancedConfig(std::ostream& stream, int subsystemID);
    /**
     * @brief writes the preamble comment (saying that it is an automatically generated file)
     *
     * @param stream destination stream
     */
    void writePreamble(std::ostream& stream);
    /**
     * @brief writes the header includes
     *
     * @param stream destination stream
     */
    void writeHeaderIncludes(std::ostream& stream);

    /**
     * @brief puts a header file into the list of used headers
     *
     * @param headerName name of the header file
     * @param global true if the file is a global fpga-log file, false for local project headers
     */
    void putUsedHeader(std::string headerName, bool global);

    /**
     * @brief writes an object initialization, i.e. the call of its init function
     *
     * This function checks if all parameter objects are initialized and if not it initializes them recursively.
     *
     * @param stream destination stream
     * @param object object to initalize
     * @param objects map of all objects in the system
     * @param initDone map indicating for each object whether or not the initialization is done
     */
    void writeObjectInit(std::ostream& stream, CObject* object, std::map<std::string, CObject *> &objects, std::map<std::string, bool>& initDone);
    /**
     * @brief writes a method call
     *
     * @param stream destination stream
     * @param object object on which the method is called
     * @param method method to write
     * @param objects map of all objects in the system
     */
    void writeMethod(std::ostream& stream, CObject *object, CMethod* method, std::map<std::string, CObject *> &objects);

    /**
     * @brief writes an attribute element in the SpartanMC system XML file
     *
     * @param writer destination XML writer
     * @param id attribute ID
     * @param text attribute data
     */
    void writeAttributeElement(QXmlStreamWriter& writer, QString id, QString text);
    /**
     * @brief writes the target node in the SpartanMC system XML file
     *
     * @param writer destination XML writer
     */
    void writeTargetNode(QXmlStreamWriter& writer);

    /**
     * @brief writes a connection node in the SpartanMC system XML file
     *
     * @param writer destination XML writer
     * @param destination connection destination
     * @param lsb lsb bit of the connection (first bit)
     */
    void writeConnection(QXmlStreamWriter& writer, std::string destination, int lsb);
    /**
     * @brief writes a port node in the SpartanMC system XML file
     *
     * @param writer destination XML writer
     * @param port port name
     * @param destination connection destination
     * @param lsb lsb bit of the connection (first bit)
     */
    void writePortConnection(QXmlStreamWriter& writer, std::string port, std::string destination, int lsb);
    /**
     * @brief writes a parameter node in the SpartanMC system XML file
     *
     * @param writer destination XML writer
     * @param parameter parameter to write
     */
    void writeParameter(QXmlStreamWriter& writer, CParameter* parameter);
    /**
     * @brief writes a peripheral to the SpartanMC system XML file
     *
     * @param writer destination XML writer
     * @param peripheral peripheral to write
     * @param subsystemID ID of the subsystem the peripheral belongs to
     * @param subsystemNumber number of the subsystem the peripheral belongs to
     */
    void writePeripheral(QXmlStreamWriter& writer, SpmcPeripheral* peripheral, std::string subsystemID, int subsystemNumber);
    /**
     * @brief writes the timestamp generator peripheral to the SpartanMC system XML file
     *
     * @param writer destination XML writer
     * @param subsystemID ID of the subsystem the peripheral belongs to
     * @param subsystemNumber number of the subsystem the peripheral belongs to
     */
    void writeTimestampGen(QXmlStreamWriter& writer, std::string subsystemID, int subsystemNumber);
    /**
     * @brief writes all peripheral of a subsystem to the SpartanMC system XML file
     *
     * @param writer destination XML writer
     * @param subsystemID ID of the subsystem the peripheral belongs to
     * @param subsystemNumber number of the subsystem the peripheral belongs to
     */
    void writePeripherals(QXmlStreamWriter& writer, std::string subsystemID, int subsystemNumber);
    /**
     * @brief writes the connections of a peripheral to the SpartanMC bus to the SpartanMC system XML file
     *
     * @param writer destination XML writer
     * @param subsystemID ID of the subsystem the peripheral belongs to
     */
    void writeSpmcConnections(QXmlStreamWriter& writer, std::string subsystemID);
    /**
     * @brief writes the DMA connections of a peripheral to the SpartanMC bus to the SpartanMC system XML file
     *
     * @param writer destination XML writer
     * @param subsystemID ID of the subsystem the peripheral belongs to
     */
    void writeDMAConnections(QXmlStreamWriter& writer, std::string subsystemID);
    /**
     * @brief writes the connections to FPGA pins of the timestamp generator peripheral to the SpartanMC system XML file
     *
     * @param timestampPinPort port for pin connections of the timestamp generator
     * @param id number of the subsystem
     */
    void addTimestampPinConnections(PeripheralPort *timestampPinPort, int id);
    /**
     * @brief writes a connection to the systems clock source to the SpartanMC system XML file
     *
     * @param stream destination stream
     * @param destination destination clock signal
     */
    void writeClkConnection(std::ostream& stream, std::string destination);

    /**
     * @brief writes the clock FPGA pin which delivers the system clock) to the SpartanMC system XML file
     *
     * @param writer destination XML writer
     */
    void writeClkPin(QXmlStreamWriter& writer);
    /**
     * @brief writes the FPGA pin definitions to the SpartanMC system XML file
     *
     * @param writer destination XML writer
     */
    void writePins(QXmlStreamWriter& writer);

    /**
     * @brief calculates the set of used processor IDs, it is stored in the processorSet variable afterwards
     */
    void calculateUsedProcessorIDs();

    /**
     * @brief defines a peripheral for system simulation (on PC) in the pc_peripherals header file
     *
     * @param name name of the peripheral
     * @param dataType data type of the peripheral
     */
    void definePeripheralForSimulation(std::string name, DataType* dataType);

    /**
     * @brief executes a command asynchronously in the background
     *
     * Usually commands are shell commands, except for the keyword "generateSystemXML(false)" which calls the corresponding method
     *
     * @param cmd command to execute
     */
    void exec(std::string cmd);

    /**
     * @brief checks an output message of the synthesis tool for success and errors
     *
     * @param message synthesis output message
     */
    void checkSynthesisMessage(std::string message);
    /**
     * @brief checks a message for the firmware size value (output of measureFirmwareSize child process) and parses it, triggers firmwareSizeResult signal
     *
     * @param message message to check
     */
    void checkFirmwareSizeMessage(std::string message);

    /**
     * @brief gets the last line of a file
     *
     * @param fileName file name
     * @return the last line of a file
     */
    std::string readLastLine(std::string fileName);
    /**
     * @brief determines the minimum number of BlockRAMs needed for a subsystem from the ucf file
     *
     * To enable this method a compilation of the same system has to be run before.
     *
     * @param cpuID number of the subsystem
     * @return mimimum required BlockRAM count
     */
    int determineMinBlockRAMcount(int cpuID);
    /**
     * @brief determines the core which should connect to the timestamp_counter peripheral (i.e. can set the system time), the result is stored in timestampCounterPeripheralCore
     */
    void determineTimestampCounterPeripheralCore();

    DataLogger* dataLogger; /**< datalogger we work on */

    std::string directory; /**< system working directory */

    std::set<std::string> usedHeaders; /**< set of used header files (which need to be included) */

    int usedIdCounter; /**< counter to generate unique IDs */
    std::list<FpgaPin> usedPins; /**< list of used FPGA pins */
    std::map<int, int> usedTimestampSources; /**< map of used timestamp sources, key: subsystem -> value: number of sources */
    std::map<int, int> usedTimestampPinSources; /**< map of used timestamp pin sources, key: subsystem -> value: number of pin sources */
    int timestampCounterPeripheralCore; /**< id of the core which should connect to the timestamp_counter peripheral (i.e. can set the system time) */

    QProcess process; /**< external process execution object */
    std::list<std::string> pending; /**< list of pending commands to execute */
    bool busy; /**< generator busy flag (writing or an external tool is working) */

    bool error; /**< error flag indicating whether or not an error occured in the current task execution */

    int pcPeripheralIdCounter; /**< counter to generate unique peripheral IDs for simulation */
    std::stringstream pcPeripheralsStream; /**< stream of simulation peripheral definitions */
    std::map<int,std::stringstream> pcTimestampCaptureStream; /**< timestamp capture signals map, key: subsystem */
    int pcPeripheralCompareCounter; /**< counter for number of used compare peripherals (to generate simulation definitions) */
    int pcPeripheralTimerCounter; /**< counter for number of used timer peripherals (to generate simulation definitions) */

    long timestampInvertMask; /**< mask of inverted timestamp signals */

    bool timingError; /**< timing error flag to indicate if a timing error occured in the current synthesis */
    bool synthesisSuccessful; /**< synthesis success flag */

    std::set<int> processorSet; /**< set of used processors (subsystem numbers) */

    bool optimizeBlockRAMcount; /**< flag to configure whether or not two passes should be performed to minimize the BlockRAM count */

    bool firmwareSizeMeasurement; /**< flag indicating if a firmware size measurement is currently in progress */
private slots:
    /**
     * @brief slot triggered when a child process (e.g. synthesis) has written new data to its stdout
     */
    void newChildStdOut();
    /**
     * @brief slot triggered when a child process (e.g. synthesis) has written new data to its stderr
     */
    void newChildErrOut();
    /**
     * @brief slot triggered when a child process finished
     */
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
