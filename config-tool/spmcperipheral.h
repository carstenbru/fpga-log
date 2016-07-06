/**
 * @file spmcperipheral.h
 * @brief SpmcPeripheral and PeripheralPort classes
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef SPMCPERIPHERAL_H
#define SPMCPERIPHERAL_H

#include <QObject>
#include <list>
#include <string>
#include <map>
#include <QXmlStreamReader>
#include "datatype.h"
#include "cparameter.h"

//CObject forward declariation,including the header here would lead to a cycle
class CObject;
//SpmcPeripheral forward declariation, as PeripheralPort and SpmcPeripheral need each other mutually
class SpmcPeripheral;

/**
 * @brief class encapsulating a connection port of a peripheral
 */
class PeripheralPort : public QObject {
    Q_OBJECT

public:
    /**
     * @brief main constructor for a port of width one
     *
     * @param name name of the port
     */
    PeripheralPort(std::string name);

    /**
     * @brief constructor for a port with variable width
     *
     * @param name name of the port
     * @param widthRef parameter defining the width of this port
     */
    PeripheralPort(std::string name, CParameter* widthRef);

    /**
     * @brief constructor for a port with fixed width
     * @param name name of the port
     * @param width width of the port
     */
    PeripheralPort(std::string name, int width);
    ~PeripheralPort();

    /**
     * @brief loads a peripheral port from a description
     *
     * @param in desciption in XML reader
     * @param parent parent object where the peripheral of this port is located (to notify about an updated module definition)
     * @param peripheral peripheral the port belongs to (needed for variable width references)
     */
    void load(QXmlStreamReader& in, CObject *parent, SpmcPeripheral *peripheral);

    /**
     * @brief sets a line of a port to a new value
     *
     * @param newValue the new value of that line
     * @return true if the line could be found and was updated successfully
     */
    bool setLine(CParameter* newValue);

    /**
     * @brief sets the direction of the port
     *
     * @param direction new port direction
     */
    void setDirection(std::string direction);

    /**
     * @brief gets a list of parameters, one parameter per line of this port
     *
     * @return a list of line parameters
     */
    std::list<CParameter*> getLines() { return lines; }

    /**
     * @brief gets the name of the port
     *
     * @return the name of the port
     */
    std::string getName() { return name; }

    /**
     * @brief gets the direction of the port
     *
     * @return the direction of the port
     */
    std::string getDirection() { return direction; }

    /**
     * @brief sets whether or not the parameter should be hidden from the user (i.e. not show in GUI)
     *
     * @param hide true to hide the parameter from the user
     */
    void setHideFromUser(bool hide);

    /**
     * @brief sets the physical constraints for pins assigned to this port (e.g. PULLUP)
     * @param constraints new physical constraints
     */
    void setConstraints(std::string constraints) { this->constraints = constraints; }

    /**
     * @brief gets the physical constraints for pins assigned to this port (e.g. PULLUP)
     *
     * @return the physical constraints
     */
    std::string getConstraints() { return constraints; }

    /**
     * @brief gets a possible port width reference
     *
     * @return the port width reference
     */
    std::string getWidthRef() { return widthRef; }

    /**
     * @brief sets the description for all parameters of all lines of this port
     */
    void setDescription(std::string description);

    /**
     * @brief serialization operator, writes the port to a XML stream
     * @param out XML stream destination
     * @param port port to write
     * @return destination XML stream for operator concatenation
     */
    friend QXmlStreamWriter& operator<<(QXmlStreamWriter& out, PeripheralPort& port);
private:
    std::string name; /**< port name */
    std::string direction; /**< port direction */
    std::string description; /**< description (e.g. to show in GUI) of all lines */

    std::string widthRef; /**< port width reference to a parameter */

    std::list<CParameter*> lines; /**< port line parameters (connections) */

    std::string constraints; /**< port constraints for destination pins of this port */
public slots:
    /**
     * @brief slot to notify about a port width change
     *
     * @param widthVal new port width value
     */
    void newWidth(std::string widthVal);
};

/**
 * @brief class representing a SpartanMC peripheral
 */
class SpmcPeripheral
{
public:
    /**
     * @brief main constructor
     *
     * @param name name of the new peripheral
     * @param dataType datatype of the peripheral
     * @param parentObject parent object to which the peripheral belongs
     * @param dataLogger datalogger to which the peripheral belongs
     */
    SpmcPeripheral(std::string name, DataType* dataType, CObject* parentObject, DataLogger *dataLogger);

    /**
     * @brief constructor loading a peripheral from a description
     * @param in XML description reader
     * @param parentObject parent object to which the peripheral belongs
     * @param dataLogger datalogger to which the peripheral belongs
     * @param ignorePins true to not load pin assignments (e.g. used for module copy as the new module should not have the same pins assigned)
     */
    SpmcPeripheral(QXmlStreamReader& in, CObject* parentObject, DataLogger *dataLogger, bool ignorePins = false);
    ~SpmcPeripheral();

    /**
     * @brief gets the parameters for this peripheral
     *
     * @return a list of parameters for this peripheral
     */
    std::list<CParameter*> getParameters() { return parameters; }

    /**
     * @brief gets a parameter by name
     *
     * @param name name of the parameter to obtain
     * @return the requested parameter
     */
    CParameter* getParameter(std::string name);

    /**
     * @brief updates a parameter
     *
     * First the parameter is seached by comparison of the signature, if it cannot be found by name for the case of an updated datatype.
     *
     * @param newValue new value of the parameter
     * @return true if the parameter was found and updated successfully
     */
    bool setParameter(CParameter* newValue);

    /**
     * @brief gets a port of the peripheral
     *
     * @param group group name of the port
     * @param name name of the port
     * @return the requested port
     */
    PeripheralPort* getPort(std::string group, std::string name);

    /**
     * @brief sets the first port line of a specified port
     *
     * @param group group name of the port
     * @param name name of the port
     * @param value new value oft the line
     */
    void setFirstPortLine(std::string group, std::string name, std::string value);

    /**
     * @brief gets a map of all ports of the peripheral
     *
     * @return a map of a list of all peripheral ports with the group name as key for the map
     */
    std::map<std::string, std::list<PeripheralPort*> > getPorts() { return ports; }

    /**
     * @brief gets the complete name of the peripheral
     *
     * @return the complete name of the peripheral
     */
    std::string getCompleteName();

    /**
     * @brief the peripherals parent name
     *
     * @return the peripherals parent name
     */
    std::string getParentName();

    /**
     * @brief the peripherals parent object
     *
     * @return the peripherals parent object
     */
    CObject* getParentObject() { return parentObject; }

    /**
     * @brief gets the datatype of the peripheral
     * @return the datatype of the peripheral
     */
    DataType* getDataType() { return dataType; }

    /**
     * @brief checks whether or not the module uses DMA
     *
     * @return true if the module uses DMA
     */
    bool hasDMA() {return hasDMAmemory; }

    /**
     * @brief loads all SpartanMC peripheral XML files
     */
    static void loadPeripheralXMLs();

    /**
     * @brief serialization operator, writes the peripheral to a XML stream
     * @param out XML stream destination
     * @param peripheral peripheral to write
     * @return destination XML stream for operator concatenation
     */
    friend QXmlStreamWriter& operator<<(QXmlStreamWriter& out, SpmcPeripheral& peripheral);
private:
    /**
     * @brief gets the SpartanMC peripheral XML file name
     *
     * @return the SpartanMC peripheral XML file name
     */
    std::string getFileName();
    /**
     * @brief reads a parameter element in a SpartanMC peripheral XML file
     *
     * @param reader source reader
     */
    void readParameterElement(QXmlStreamReader& reader);
    /**
     * @brief reads a ports element in a SpartanMC peripheral XML file
     *
     * @param reader source reader
     * @param direction port direction obtained before (as attribute in parent tag)
     */
    void readPortsElement(QXmlStreamReader& reader, std::string direction);
    /**
     * @brief reads the SpartanMC peripheral XML file for this peripheral
     */
    void readPeripheralXML();
    /**
     * @brief reads additional information about this peripheral from the parent objects fpga-log module XML file
     */
    void readModuleXML();

    /**
     * @brief reads the peripheral name from a SpartanMC peripheral XML file
     *
     * @param fileName file name
     * @return the requested peripheral name
     */
    static std::string readModuleNameFromFile(std::string fileName);

    /**
     * @brief saves the peripheral to a fpga-log XML file
     *
     * @param out destination writer
     */
    void saveToXml(QXmlStreamWriter& out);

    std::string name; /**< peripheral name */
    DataLogger* dataLogger; /**< parent datalogger */
    bool hasDMAmemory; /**< flag indicating if the peripheral uses DMA */

    DataType* dataType; /**< datatype of the peripheral */
    std::list<CParameter*> parameters; /**< list of all peripheral parameters (e.g. pins, baudrate,...) */
    std::map<std::string, std::list<PeripheralPort*> > ports; /**< map of all peripheral ports (key: group, value: list of ports in this group) */

    CObject* parentObject; /**< parent object */

    static std::map<std::string, std::string> peripheralXMLs; /**< map of all peripheral XML files (key: peripheral type, value: XML file) */
};

#endif // SPMCPERIPHERAL_H
