#ifndef PORT_H
#define PORT_H

#include <QObject>
#include <string>
#include <map>
#include <QXmlStreamWriter>
#include <QPoint>

typedef std::pair<std::string, std::string> stringPair;

class DatastreamObject;

typedef enum {
    PORT_TYPE_DATA_OUT = 0,
    PORT_TYPE_CONTROL_IN,
    PORT_TYPE_DATA_IN,
    PORT_TYPE_CONTROL_OUT
} port_type;

class Port : public QObject
{
    Q_OBJECT

public:
    Port(std::string name, DatastreamObject* parent) : name(name), parent(parent) {}
    virtual ~Port() { disconnectPort(); }

    virtual int connectPort(Port* port) = 0;
    virtual void disconnectPort() { emit disconnectFromDest(); }
    std::string getName() { return name; }

    DatastreamObject* getParent() { return parent; }

    virtual void saveToXml(QXmlStreamWriter& out, std::string type);
protected:
    std::string name;

    DatastreamObject* parent;
signals:
    void connected();
    void disconnected(Port* port);
    void viaChanged();

    void disconnectFromDest();
};

class ControlPortIn : public Port
{
public:
    ControlPortIn(std::string name, DatastreamObject* parent) : Port(name, parent) {}
    virtual ~ControlPortIn() {}

    virtual int connectPort(Port *port);
};

class DataPortIn : public Port
{
public:
    DataPortIn(std::string name, DatastreamObject* parent) : Port(name, parent) {}
    virtual ~DataPortIn() {}

    virtual int connectPort(Port* port);
};

class PortOut : public Port
{
    Q_OBJECT

public:
    PortOut(std::string name, DatastreamObject* parent) : Port(name, parent), destination(NULL), multiPort(false) {}
    PortOut(std::string name, DatastreamObject* parent, bool multiPort) : Port(name, parent), destination(NULL), multiPort(multiPort) {}
    virtual ~PortOut() {}
    Port* getDestination() { return destination; }
    virtual void disconnectPort();

    void load(QXmlStreamReader& in, std::map<PortOut*, stringPair>& connections);

    bool isConnected() { return destination != NULL; }
    bool isMultiPort() { return multiPort; }

    void addVia(QPoint via, QPoint& prev);
    void moveVia(QPoint oldPos, QPoint newPos);
    void deleteVia(QPoint pos);
    std::list<QPoint> getVias() { return vias; }

    virtual void saveToXml(QXmlStreamWriter& out, std::string type);
protected:
    Port* destination;

    bool multiPort;

    std::list<QPoint> vias;
private slots:
    void destDisconnected();
};

class ControlPortOut : public PortOut
{
public:
    ControlPortOut(std::string name, DatastreamObject* parent) : PortOut(name, parent) {}
    ControlPortOut(std::string name, DatastreamObject* parent, bool multiPort) : PortOut(name, parent, multiPort) {}
    virtual ~ControlPortOut() {}

    virtual int connectPort(Port* port);
};

class DataPortOut : public PortOut
{
public:
    DataPortOut(std::string name, DatastreamObject* parent) : PortOut(name, parent) {}
    DataPortOut(std::string name, DatastreamObject* parent, bool multiPort) : PortOut(name, parent, multiPort) {}
    virtual ~DataPortOut() {}

    virtual int connectPort(Port* port);
};

QXmlStreamWriter& operator<<(QXmlStreamWriter& out, ControlPortIn& port);
QXmlStreamWriter& operator<<(QXmlStreamWriter& out, ControlPortOut& port);
QXmlStreamWriter& operator<<(QXmlStreamWriter& out, DataPortIn& port);
QXmlStreamWriter& operator<<(QXmlStreamWriter& out, DataPortOut& port);

#endif // PORT_H
