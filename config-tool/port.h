/**
 * @file port.h
 * @brief various port classes (module connections)
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef PORT_H
#define PORT_H

#include <QObject>
#include <string>
#include <map>
#include <QXmlStreamWriter>
#include <QPoint>

typedef std::pair<std::string, std::string> stringPair;

class DatastreamObject;

/**
 * @brief enumeration of the different port types
 */
typedef enum {
    PORT_TYPE_DATA_OUT = 0, /**< data output port */
    PORT_TYPE_CONTROL_IN, /**< control input port */
    PORT_TYPE_DATA_IN, /**< data input port */
    PORT_TYPE_CONTROL_OUT /**< control output port */
} port_type;

/**
 * @brief port superclass representing all kinds of ports for communication between modules
 */
class Port : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief constructor
     * @param name port name
     * @param parent parent module
     */
    Port(std::string name, DatastreamObject* parent) : name(name), parent(parent) {}
    virtual ~Port() { disconnectPort(); }

    /**
     * @brief connects another port with this port
     *
     * @param port other port of the connection
     * @return 1 on success, otherwise 0
     */
    virtual int connectPort(Port* port) = 0;

    /**
     * @brief disconnectes the port (removes the conenction)
     */
    virtual void disconnectPort() { emit disconnectFromDest(); }

    /**
     * @brief gets the name of the port
     * @return the name of the port
     */
    std::string getName() { return name; }

    /**
     * @brief gets the parent object of the port, i.e. its module
     *
     * @return the parent object of the port, i.e. its module
     */
    DatastreamObject* getParent() { return parent; }

    /**
     * @brief saves the port to a XML description
     *
     * @param out destination XML writer
     * @param type port type
     */
    virtual void saveToXml(QXmlStreamWriter& out, std::string type);
protected:
    std::string name; /**< the name of the port */

    DatastreamObject* parent; /**< the parent moduke this port belongs to */
signals:
    /**
     * @brief emitted when the port is connected to another port
     */
    void connected();

    /**
     * @brief emitted when the port is disconnected
     * @param port this port
     */
    void disconnected(Port* port);

    /**
     * @brief emitted when the VIA on the connection of the port changed
     */
    void viaChanged();

    /**
     * @brief emitted when the destination disconnected
     */
    void disconnectFromDest();
};

/**
 * @brief class representing a control input port
 */
class ControlPortIn : public Port
{
public:
    /**
     * @brief constructor
     * @param name port name
     * @param parent parent module
     */
    ControlPortIn(std::string name, DatastreamObject* parent) : Port(name, parent) {}
    virtual ~ControlPortIn() {}

    virtual int connectPort(Port *port);
};

/**
 * @brief class representing a data input port
 */
class DataPortIn : public Port
{
public:
    /**
     * @brief constructor
     * @param name port name
     * @param parent parent module
     */
    DataPortIn(std::string name, DatastreamObject* parent) : Port(name, parent) {}
    virtual ~DataPortIn() {}

    virtual int connectPort(Port* port);
};

/**
 * @brief class representing a output port, i.e. a sending port
 */
class PortOut : public Port
{
    Q_OBJECT

public:
    /**
     * @brief constructor
     * @param name port name
     * @param parent parent module
     */
    PortOut(std::string name, DatastreamObject* parent) : Port(name, parent), destination(NULL), multiPort(false) {}

    /**
     * @brief constructor
     * @param name port name
     * @param parent parent module
     * @param multiPort true if the port is related to a group of ports
     */
    PortOut(std::string name, DatastreamObject* parent, bool multiPort) : Port(name, parent), destination(NULL), multiPort(multiPort) {}
    virtual ~PortOut() {}

    /**
     * @brief gets the destination side port (connected port)
     *
     * @return the destination side port (connected port)
     */
    Port* getDestination() { return destination; }
    virtual void disconnectPort();

    /**
     * @brief loads the port from an XML, adds a connection to the map connections if the port is connected
     *
     * The connection is just added to the map for now as the destinatio port is probably loaded later so we cannot connect the ports yet.
     *
     * @param in XML stream reader
     * @param connections map of connections which should be made later (after loading all ports)
     */
    void load(QXmlStreamReader& in, std::map<PortOut*, stringPair>& connections);

    /**
     * @brief checks whether the port is connected
     * @return true if the port is connected
     */
    bool isConnected() { return destination != NULL; }

    /**
     * @brief checks whether the port is a multiPort (i.e. belonging to a group of ports)
     * @return true if the port is a multiPort
     */
    bool isMultiPort() { return multiPort; }

    /**
     * @brief adds a VIA to the connection between this port and its destination
     *
     * @param via new VIA postion
     * @param prev VIA after which the new VIA should be inserted
     */
    void addVia(QPoint via, QPoint& prev);

    /**
     * @brief moves a VIA on a connection
     *
     * @param oldPos old VIA position
     * @param newPos new VIA position
     */
    void moveVia(QPoint oldPos, QPoint newPos);

    /**
     * @brief deletes a VIA
     * @param pos position of the VIA to delete
     */
    void deleteVia(QPoint pos);

    /**
     * @brief gets a list of all VIAs
     *
     * @return a list of all VIAs
     */
    std::list<QPoint> getVias() { return vias; }

    virtual void saveToXml(QXmlStreamWriter& out, std::string type);
protected:
    Port* destination; /**< destination port of the connection */

    bool multiPort; /**< multi port flag */

    std::list<QPoint> vias; /**< list of VIA positions on the connection */
private slots:
    void destDisconnected();
};

/**
 * @brief class representing a control output port
 */
class ControlPortOut : public PortOut
{
public:
    /**
     * @brief constructor
     * @param name port name
     * @param parent parent module
     */
    ControlPortOut(std::string name, DatastreamObject* parent) : PortOut(name, parent) {}

    /**
     * @brief constructor
     * @param name port name
     * @param parent parent module
     * @param multiPort true if the port is related to a group of ports
     */
    ControlPortOut(std::string name, DatastreamObject* parent, bool multiPort) : PortOut(name, parent, multiPort) {}
    virtual ~ControlPortOut() {}

    virtual int connectPort(Port* port);
};

/**
 * @brief class representing a data output port
 */
class DataPortOut : public PortOut
{
public:
    /**
     * @brief constructor
     * @param name port name
     * @param parent parent module
     */
    DataPortOut(std::string name, DatastreamObject* parent) : PortOut(name, parent) {}

    /**
     * @brief constructor
     * @param name port name
     * @param parent parent module
     * @param multiPort true if the port is related to a group of ports
     */
    DataPortOut(std::string name, DatastreamObject* parent, bool multiPort) : PortOut(name, parent, multiPort) {}
    virtual ~DataPortOut() {}

    virtual int connectPort(Port* port);
};

QXmlStreamWriter& operator<<(QXmlStreamWriter& out, ControlPortIn& port);
QXmlStreamWriter& operator<<(QXmlStreamWriter& out, ControlPortOut& port);
QXmlStreamWriter& operator<<(QXmlStreamWriter& out, DataPortIn& port);
QXmlStreamWriter& operator<<(QXmlStreamWriter& out, DataPortOut& port);

#endif // PORT_H
