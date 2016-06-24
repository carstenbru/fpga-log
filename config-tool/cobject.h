/**
 * @file cobject.h
 * @brief CObject class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef COBJECT_H
#define COBJECT_H

#include <QObject>
#include <string>
#include <list>
#include "datatype.h"
#include "spmcperipheral.h"

/**
 * @brief class representing a datalogger object (modules and other objects)
 */
class CObject : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief main constructor
     *
     * Objects are usually created over the DataLogger class which calls this constructor.
     *
     * @param name name of the new object
     * @param dataType datatype of the new object
     * @param dataLogger the datalogger the new object should belong to
     */
    CObject(std::string name, DataTypeStruct *dataType, DataLogger* dataLogger);

    /**
     * @brief constructor loading an object from a description
     *
     * @param in XML description reader
     * @param dataLogger the datalogger the new object should belong to
     * @param name name of the new object
     * @param ignorePins true to not load pin assignments (e.g. used for module copy as the new module should not have the same pins assigned)
     */
    CObject(QXmlStreamReader& in, DataLogger* dataLogger, std::string name = "", bool ignorePins = false);

    /**
     * @brief constructor loading an object from a description
     *
     * This constructor has a dummy parameter (bool) to enable the DatastreamObject class to move the XML reader in the initialization list.
     * It is doing exactly the same as the other XML loading constructor.
     *
     * @param in XML description reader
     * @param dataLogger the datalogger the new object should belong to
     * @param name name of the new object
     * @param ignorePins true to not load pin assignments (e.g. used for module copy as the new module should not have the same pins assigned)
     */
    CObject(QXmlStreamReader& in, DataLogger* dataLogger, bool, std::string name = "", bool ignorePins = false);
    virtual ~CObject();

    /**
     * @brief adds a new advanced configuration method call
     *
     * @param methodName name of the method call to add
     */
    void addAdvancedConfig(std::string methodName);

    /**
     * @brief gets the name of the object
     *
     * @return the name of the object
     */
    std::string getName() { return name; }

    /**
     * @brief sets the name of the object
     * @param name new name of the object
     */
    void setName(std::string name) { this->name = name; }

    /**
     * @brief gets the datatype of the object
     *
     * @return the datatype of the object
     */
    DataTypeStruct* getType() { return type; }

    /**
     * @brief gets the initialization method which has to be called to initialize (in the logger code) this object
     * @return the initialization method for this object
     */
    CMethod* getInitMethod() { return initMethod; }

    /**
     * @brief gets a list of advanced configuration method calls
     *
     * @return a list of advanced configuration method calls
     */
    std::list<CMethod*> getAdvancedConfig() { return advancedConfig; }

    /**
     * @brief gets the SpartanMC peripherals used by this object
     *
     * @return the SpartanMC peripherals used by this object
     */
    std::list<SpmcPeripheral*> getPeripherals() { return peripherals; }

    /**
     * @brief gets the timestamp pins (pins directly connected to the timestamp generator) used by this object
     *
     * @return the timestamp pins used by this object as map
     */
    std::map<std::string, CParameter*> getTimestampPins() { return timestampPins; }

    /**
     * @brief gets the parameters used to assign pins to timestamp pins as list
     *
     * @return a list of parameters for timestamp pins
     */
    std::list<CParameter*> getTimestampPinParameters();

    /**
     * @brief checks if a timestamp pin should be used in inverted form
     *
     * @param pin the pin to check
     * @return true if the timestamp pin should be inverted
     */
    bool getTimestampPinInvert(std::string pin) { return timestampPinsInvert.at(pin); }

    /**
     * @brief sets a flag indicating whether the objects definition has been altered (e.g. between saving the logger and loading it)
     * @see definitionsUpdated
     *
     * @param state new state of the flag
     */
    void setDefinitionsUpdated(bool state) {definitionsUpdated = state;}

    /**
     * @brief gets a flag indicating whether the objects definition has been altered (e.g. between saving the logger and loading it)
     * @see definitionsUpdated
     *
     * @return true if the objects definition has been altered
     */
    bool getDefinitionsUpdated() {return definitionsUpdated;}

    /**
     * @brief sets the SpartanMC processor core ID this module should run on
     *
     * @param core new SpartanMC core
     */
    void setSpartanMcCore(int core) { spartanMcCore = core; }

    /**
     * @brief gets the SpartanMC processor core ID this module should run on
     * @return the SpartanMC processor core ID
     */
    int getSpartanMcCore() { return spartanMcCore; }

    /**
     * @brief checks whether the module can alter the dataloggers system time or not
     *
     * It is not checked whether or not the object really alters the system time in this particular logger,
     * just if the module is capable of doing so.
     *
     * @return true if the object can (!) alter the dataloggers system time
     */
    bool getWritesSystemTime() { return writesSystemTime; }

    /**
     * @brief serialization operator, writes the object to a XML stream
     *
     * @param out XML stream destination
     * @param cObject object to write
     * @return destination XML stream for operator concatenation
     */
    friend QXmlStreamWriter& operator<<(QXmlStreamWriter& out, CObject& cObject);
protected:
    /**
     * @brief reads the list of timestamp pins from the objects module XML definition file
     */
    void readTimestampPinsFromModuleXml();

    /**
     * @brief updates a peripheral
     *
     * The peripheral name is compared with all existing peripherals and if an equal one is found the new peripheral replaces it
     *
     * @param newPeripheral updated peripheral
     * @return true if the peripheral was found in the list of peripherals and it was replaced
     */
    bool setPeripheral(SpmcPeripheral* newPeripheral);

    std::string name; /**< @brief object name */
    DataTypeStruct* type; /**< @brief datatype of the object */

    CMethod* initMethod; /**< @brief initialization method of the object */
    std::list<SpmcPeripheral*> peripherals; /**< @brief list of all peripherals of the object */
    std::map<std::string, CParameter*> timestampPins; /**< @brief map of all timestamp pins */
    std::map<std::string, bool> timestampPinsInvert; /**< @brief map indicating which timestamp pins are used in inverted form */

    std::list<CMethod*> advancedConfig; /**< @brief list of advanced configuration method calls */

    bool definitionsUpdated; /**< @brief flag indicating whether the objects definition has been altered (e.g. between saving the logger and loading it) */

    int spartanMcCore; /**< @brief DpartanMC core ID this object is currently assigned to */
    bool writesSystemTime; /**< @brief flag indicating whether or not this object can (!) change the loggers system time */
public slots:
    /**
     * @brief slot for advanced config method removal signals
     *
     * @param methodId id (number) of the method to remove
     */
    void removeAdvancedConfig(int methodId);
signals:
    /**
     * @brief signal triggered when an advanced config method call was removed
     */
    void advancedConfigRemoved();
};

#endif // COBJECT_H
