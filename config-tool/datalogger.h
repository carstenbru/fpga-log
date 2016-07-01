/**
 * @file datalogger.h
 * @brief DataLogger class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <QObject>
#include <list>
#include <vector>
#include <QXmlStreamWriter>
#include "datatype.h"
#include "cobject.h"
#include "datastreamobject.h"
#include "automaticcoreassigner.h"

/**
 * @brief class representing a datalogger
 */
class DataLogger : public QObject
{
    Q_OBJECT

public:
    DataLogger();
    DataLogger(DataLogger& other);
    ~DataLogger();

    /**
     * @brief adds a new object of a given type to the system
     *
     * The object is initialized with the default values.
     * The name is the datatype appended with the smallest possible number leading to no conflicts with other modules.
     *
     * @param type type of the new object
     * @return the auto-generated name of the object
     */
    std::string newObject(DataTypeStruct *type);

    /**
     * @brief adds a previously created DatastreamObject to the datalogger
     *
     * @param dso DatastreamObject to add
     */
    void addDataStreamObject(DatastreamObject *dso);

    /**
     * @brief adds an object to the dataLogger which is described in XML format (as they are saved in files)
     *
     * @param name name of the new object
     * @param isDataStreamObject true for dataStream object, false for other modules
     * @param description XML description (as XML-Reader) of the module
     * @param pos position of the module (only for dataStream objects)
     */
    void addObject(std::string name, bool isDataStreamObject, QXmlStreamReader& description, QPoint pos = QPoint(0,0));

    /**
     * @brief deletes an object from the logger
     *
     * @param object pointer determining the object
     */
    void deleteObject(CObject* object);

    /**
     * @brief gets a list of all datastream modules currently in the logger
     *
     * @return a list of all datastream modules currently in the logger
     */
    std::list<DatastreamObject*> getDatastreamModules() { return datastreamObjects; }

    /**
     * @brief gets a list of all other modules (not dataStream modules) currently in the logger
     *
     * @return a list of all other modules (not dataStream modules) currently in the logger
     */
    std::vector<CObject*> getOtherObjects() { return otherObjects; }

    /**
     * @brief gets a list of all instances of a specific type (including subclasses)
     *
     * @param dataType data type of instances to obtain
     * @return a list of all instances of a specific type (including subclasses)
     */
    std::list<CObject*> getInstances(DataTypeStruct *dataType);

    /**
     * @brief gets a map of all objects (dataStream and not dataStream) in the logger, the names of the objects are used as keys
     *
     * @return a map of all objects (dataStream and not dataStream) in the logger, the names of the objects are used as keys
     */
    std::map<std::string, CObject*> getObjectsMap();

    /**
     * @brief updates the name of a object
     *
     * @param object the object of which the name should be chaneged
     * @param newName new name of the object
     * @return true on success, false otherwise (empty name, name already in use)
     */
    bool changeObjectName(CObject* object, std::string newName);

    /**
     * @brief gets the input clock frequency (not system clock!)
     *
     * @return the input clock frequency
     */
    int getClk();

    /**
     * @brief gets the system clock frequency (CPU core frequency)
     *
     * @return the system clock frequency (CPU core frequency)
     */
    int getSystemClk();

    /**
     * @brief gets the peripheral clock frequency (usually equal to system/CPU clock)
     *
     * @return the peripheral clock frequency (usually equal to system/CPU clock)
     */
    int getPeriClk();

    /**
     * @brief gets the input clock divider which is used to derive the system clock
     *
     * @return the input clock divider which is used to derive the system clock
     */
    int getClkDivide();

    /**
     * @brief gets the input clock multiplier which is used to derive the system clock
     *
     * @return the input clock multiplier which is used to derive the system clock
     */
    int getClkMultiply();

    /**
     * @brief reads the pin definitions of the target platform from SpartanMC target XML
     */
    void loadTargetPins();

    /**
     * @brief determines where CoreConnectors modules are needed in the design (i.e. a connections between two CPU cores) and adds them to the system
     */
    void addCoreConnectors();

    /**
     * @brief adds a single core connector to the system
     *
     * The connector is added between the output port given and its destination.
     *
     * @param module module containing the output port in parameter \p port
     * @param port output port after which the connector should be added
     * @param contolStream true if the stream is a control stream, false for a data stream
     */
    void addCoreConnector(DatastreamObject* module,PortOut* port, bool contolStream);

    /**
     * @brief gets the parameter configuring the target platform
     *
     * @return the parameter configuring the target platform
     */
    CParameter* getTarget() { return &target; }

    /**
     * @brief gets the parameter configuring the input clock pin
     *
     * @return the parameter configuring the input clock pin
     */
    CParameter* getClockPin() { return &clockPin; }

    /**
     * @brief gets the parameter configuring the clock frequency
     *
     * @return the parameter configuring the clock frequency
     */
    CParameter* getClockFreq() { return &clockFreq; }

    /**
     * @brief gets the parameter configuring the clock division factor
     *
     * @return the parameter configuring the clock division factor
     */
    CParameter* getClockDivideParam() { return &clockDivide; }

    /**
     * @brief gets the parameter configuring the clock multiplier
     *
     * @return the parameter configuring the clock multiplier
     */
    CParameter* getClockMultiplyParam() { return &clockMultiply; }

    /**
     * @brief gets the parameter selecting between expert and normal mode
     *
     * @return the parameter selecting between expert and normal mode
     */
    CParameter* getExpertMode() { return &expertMode; }

    /**
     * @brief checks if the expert mode is enabled
     *
     * @return true if the expert mode is enabled
     */
    bool isExpertMode() { return expertMode.getValue().compare("TRUE") == 0; }

    /**
     * @brief sets the definitionsUpdated flag
     *
     * @param state the new state
     */
    void setDefinitionsUpdated(bool state) {definitionsUpdated = state;}

    /**
     * @brief gets the definitionsUpdated flag
     *
     * @return the definitions updated flag
     */
    bool getDefinitionsUpdated() {return definitionsUpdated;}

    /**
     * @brief adds a module to the list of updated modules, sets the definitionsUpdated flag
     * @param moduleName
     */
    void addDefinitionsUpdatedModule(std::string moduleName) {definitionsUpdatedModules += "\n" + moduleName; definitionsUpdated = true;}

    /**
     * @brief gets the names of updated (definitions changed) modules as string
     *
     * @return the names of updated (definitions changed) modules as string
     */
    std::string getDefinitionsUpdatedModules() { return definitionsUpdatedModules; }

    /**
     * @brief gets a map of pins (their parameter) and their assigned function
     *
     * The returned value in the map consists of 4 strings:\n
     * [0]: full pin name (e.g. "PORTA:A01")\n
     * [1]: assigned module (e.g. "sink_uart_0")\n
     * [2]: assigned pin group in this module (e.g. "Receiver")\n
     * [3]: function in this group (name) (e.g. "rx")
     *
     * @return a map of pins (their parameter) and their assigned function
     */
    std::map<CParameter *, std::string[4]> getPinAssignments();

    /**
     * @brief finds a name for a object of a given datatype
     *
     * The name determined is the datatype appended with the smallest possible number leading to no conflicts with other modules.
     *
     * @param isDataStreamObject true for dataStream objects, false for other objects
     * @param dataType dataType to determine a name for
     * @return a name for a object of the given datatype
     */
    std::string findObjectName(bool isDataStreamObject, DataType* dataType);

    /**
     * @brief gets the dataLoggers AutomaticCoreAssigner instance
     *
     * @return the dataLoggers AutomaticCoreAssigner instance
     */
    AutomaticCoreAssigner* getAutomaticCoreAssigner() { return &automaticCoreAssigner; }

    /**
     * @brief loads all SpartanMC target XML files to know the available targets
     */
    static void loadTragetXMLs();

    /**
     * @brief gets the filename (SpartanMC target XML) for a target
     *
     * @param target the name of the target
     * @return the filename (SpartanMC target XML) for a target
     */
    static std::string getTargetXML(std::string target) { return targetXMLs.at(target); }

    /**
     * @brief writes (stores) a datalogger in a XML writer
     *
     * @param out the destination writer
     * @param dataLogger the datalogger to store
     * @return reference to the XML writer (to chain the operator)
     */
    friend QXmlStreamWriter& operator<<(QXmlStreamWriter& out, DataLogger& dataLogger);

    /**
     * @brief reads a datalogger from a XML reader
     *
     * @param in the source reader
     * @param dataLogger the destination datalogger
     * @return reference to the XML reader (to chain the operator)
     */
    friend QXmlStreamReader& operator>>(QXmlStreamReader& in, DataLogger& dataLogger);
private:
    template <typename T>
    /**
     * @brief checks if an object with a specific name is contained in a list
     *
     * @param searchList list to search in for the objects name
     * @param name name of the object to search
     * @return true if the object name is contained in the list
     */
    bool containsObjectName(T searchList, std::string name);
    template <typename T>
    /**
     * @brief determines a valid, not yet used, object name
     *
     * @param searchList list to search in for used names
     * @param dataType data type of the new object
     * @return  determined object name
     */
    std::string findObjectName(T searchList, DataType* dataType);
    template <typename T>
    /**
     * @brief adds all instances of objects in the searchList to the destList which are subclass of dataType
     *
     * @param searchList list to search for instances
     * @param destList result list of instances
     * @param dataType superclass data type
     */
    void addInstancesToList(T searchList, std::list<CObject*>& destList,  DataTypeStruct* dataType);
    template <typename T>
    /**
     * @brief deletes an object from a list
     *
     * @param searchList list to search for the object
     * @param object object to remove
     * @return true if the object was found and deleted successfully
     */
    bool deleteObject(T& searchList, CObject* object);

    /**
     * @brief gets a datastream object by name
     *
     * @param name the name to search for
     * @return the requested datastream object
     */
    DatastreamObject* getDatastreamObject(std::string name);

    /**
     * @brief reads a target name (and just that) from a SpartanMC target XML file
     *
     * @param fileName name of the file to read
     * @return the read file name
     */
    static std::string readTargetNameFromFile(std::string fileName);

    CParameter target; /**< target platform parameter */
    CParameter clockPin; /**< clock pin configuration parameter */
    CParameter clockFreq; /**< clock frequency parameter */

    CParameter clockDivide; /**< clock division parameter */
    CParameter clockMultiply; /**< clock multiply parameter */

    CParameter expertMode; /**< parameter indicating if the GUI for datalogger should be in expert mode or not */

    std::list<DatastreamObject*> datastreamObjects; /**< list of all datastream objects in the logger */
    std::vector<CObject*> otherObjects; /**< list of all other objects (non-datastream) in the logger */

    static std::map<std::string, std::string> targetXMLs; /**< map of all targets to their XML files */

    bool definitionsUpdated; /**< definitions update flag, set if a module definition changed for a loaded logger */
    std::string definitionsUpdatedModules; /**< list of changed module definitions when loading a logger (key: name, value: file) */

    AutomaticCoreAssigner automaticCoreAssigner; /**< instance of the automatic core assigner used for the logger, includes the core assinment parameters */
private slots:
    /**
     * @brief slot to notify about a connection change between datastream modules
     */
    void moduleConnectionsChanged();
    /**
     * @brief slot for a change of a connection VIA (move, delete, add)
     */
    void viaChanged();
    /**
     * @brief slot for a change in the expert mode setting
     *
     * @param value new expert mode value
     */
    void expertModeParamChanged(std::string value);
public slots:
    /**
     * @brief slot for a change in one of the parameters of the logger
     */
    void parameterChanged();

    /**
     * @brief slot for a change in one of the pins (their parameters) of the logger
     */
    void pinParamterChanged();
signals:
    /**
     * @brief emitted when a datastrem module changed
     */
    void datastreamModulesChanged();

    /**
     * @brief emitted when a non-datastream module changed
     */
    void otherModulesChanged();

    /**
     * @brief emitted when a connection between datastream modules changed
     */
    void connectionsChanged();

    /**
     * @brief emitted when a connection VIA changed
     */
    void viasChanged();

    /**
     * @brief emitted when a critical parameter changed, i.e. a parameter referenced by something else
     */
    void criticalParameterChanged();

    /**
     * @brief emitted when a pin configuration (parameter) changed
     */
    void pinChanged();

    /**
     * @brief emitted when the pin configurations of the datalogger should be stored
     */
    void storePins();

    /**
     * @brief emitted when the expert/normal mode parameter changed
     *
     * @param expertMode the new mode (true: expert mode, false: normal mode)
     */
    void expertModeChanged(bool expertMode);
};

#endif // DATALOGGER_H
