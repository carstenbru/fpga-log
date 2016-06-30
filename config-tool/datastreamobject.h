/**
 * @file datastreamobject.h
 * @brief DatastreamObject class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef DATASTREAMOBJECT_H
#define DATASTREAMOBJECT_H

#include <string>
#include <QPoint>
#include <list>
#include "cobject.h"
#include "port.h"
#include "datatype.h"

/**
 * @brief class representing an object with data streams (or control streams)
 */
class DatastreamObject : public CObject
{
    Q_OBJECT

public:
    /**
     * @brief main constructor
     *
     * Objects are usually created over the DataLogger class which calls this constructor.
     *
     * @param name 	name of the new object
     * @param type datatype of the new object
     * @param dataLogger the datalogger the new object should belong to
     */
    DatastreamObject(std::string name, DataTypeStruct *type, DataLogger* dataLogger);

    /**
     * @brief constructor loading an object from a description, stores connections in a map instead of connecting the ports
     *
     * This constructor is used for dataLogger loading as the destination module might not exist yet
     * so the connection cannot be done yet but has to be done later.
     *
     * @param in XML description reader
     * @param dataLogger the datalogger the new object should belong to
     * @param connections map indicating connections which should be established later
     */
    DatastreamObject(QXmlStreamReader& in, DataLogger* dataLogger, std::map<PortOut *, stringPair> &connections);

    /**
     * @brief constructor loading an object from a description
     *
     * @param in XML description reader
     * @param dataLogger the datalogger the new object should belong to
     * @param name name of the new object
     * @param ignorePins true to not load pin assignments (e.g. used for module copy as the new module should not have the same pins assigned)
     */
    DatastreamObject(QXmlStreamReader& in, DataLogger* dataLogger, std::string name, bool ignorePins = false);
    ~DatastreamObject();

    /**
     * @brief gets all control input ports
     *
     * @return a list of all control input ports
     */
    std::list<ControlPortIn*> getControlInPorts() { return controlInPorts; }

    /**
     * @brief gets all control output ports
     *
     * @return a list of all control output ports
     */
    std::list<ControlPortOut*> getControlOutPorts() { return controlOutPorts; }

    /**
     * @brief gets all data input ports
     *
     * @return a list of all data input ports
     */
    std::list<DataPortIn*> getDataInPorts() { return dataInPorts; }

    /**
     * @brief gets all data output ports
     *
     * @return a list of all data output ports
     */
    std::list<DataPortOut*> getDataOutPorts() { return dataOutPorts; }

    /**
     * @brief gets all output ports of the specified type
     *
     * @param type type of the ports to get
     * @return a list of all data output ports
     */
    std::list<PortOut*> getOutPorts(port_type type);

    /**
     * @brief gets an output port by name
     *
     * @param name the name of the port to search
     * @return the requested port
     */
    PortOut* getOutPort(std::string name);

    /**
     * @brief gets a port by name
     *
     * @param name the name of the port to search
     * @return the requested port
     */
    Port* getPort(std::string name);

    /**
     * @brief sets the position of the object (graphical postion in the logger view)
     *
     * @param pos new position
     */
    void setPosition(QPoint pos);

    /**
     * @brief gets the position of the object (graphical postion in the logger view)
     *
     * @return the position of the object
     */
    QPoint getPosition();

    /**
     * @brief writes the datastreamObject to a XML stream
     * @param out destination XML writer
     * @param dObject object to write
     * @return destination XML writer, for concatination of the operator
     */
    friend QXmlStreamWriter& operator<<(QXmlStreamWriter& out, DatastreamObject& dObject);
private:
    /**
     * @brief finds the ports of the datastream module from the objects provided methods
     */
    void findPorts();
    template <typename T>
    /**
     * @brief reorders a newly unconnected port to the end of ports (for multi-ports, "add" function)
     *
     * @param ports list of ports to search in
     * @param port port to put to the end
     * @return true if the port was found
     */
    bool reorderUnconnectedPort(std::list<T*>& ports, Port* port);
    /**
     * @brief gets the first output port which is not connected and belongs to the port of "name"
     *
     * @param name name of the port to search for unconnected lines
     * @param connections map indicating connections which should be established later
     * @return the requested port
     */
    PortOut* getFirstNotConnectedOutPort(std::string name, std::map<PortOut*, stringPair>& connections);

    /**
     * @brief adds a control input port to the object
     *
     * @param port the port to add
     */
    void addPort(ControlPortIn* port);
    /**
     * @brief adds a control output port to the object
     *
     * @param port the port to add
     */
    void addPort(ControlPortOut* port);
    /**
     * @brief adds a data input port to the object
     *
     * @param port the port to add
     */
    void addPort(DataPortIn* port);
    /**
     * @brief adds a data output port to the object
     *
     * @param port the port to add
     */
    void addPort(DataPortOut* port);

    std::list<ControlPortIn*> controlInPorts; /**< list of all control input ports of the module */
    std::list<ControlPortOut*> controlOutPorts; /**< list of all control output ports of the module */
    std::list<DataPortIn*> dataInPorts; /**< list of all data input ports of the module */
    std::list<DataPortOut*> dataOutPorts; /**< list of all data output ports of the module */

    QPoint position; /**< graphical position of the module */
private slots:
    /**
     * @brief slot which should be notified when a port got connected
     */
    void portConnected();
    /**
     * @brief slot for a port disconnect
     *
     * @param port disconnected port
     */
    void portDisconnected(Port *port);
    /**
     * @brief slot for a change in the VIAs (add, delete, move)
     */
    void portViasChanged();
signals:
    /**
     * @brief emitted when a connection (either data stream or control stream) of the object changed
     */
    void connectionsChanged();
    /**
     * @brief emitted when a connection VIA adjacent to this object changes
     */
    void viasChanged();
};

#endif // DATASTREAMOBJECT_H
