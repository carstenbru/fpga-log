#ifndef PORT_H
#define PORT_H

#include <QObject>
#include <string>

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
    virtual ~Port() {}

    virtual int connectPort(Port* port) = 0;
    virtual void disconnectPort() {}
    std::string getName() { return name; }

    DatastreamObject* getParent() { return parent; }
private:
    std::string name;

    DatastreamObject* parent;
signals:
    void connected();
    void disconnected(Port* port);
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
public:
    PortOut(std::string name, DatastreamObject* parent) : Port(name, parent), destination(NULL), multiPort(false) {}
    PortOut(std::string name, DatastreamObject* parent, bool multiPort) : Port(name, parent), destination(NULL), multiPort(multiPort) {}
    virtual ~PortOut() {}
    Port* getDestination() { return destination; }
    virtual void disconnectPort();

    bool isConnected() { return destination != NULL; }
    bool isMultiPort() { return multiPort; }
protected:
    Port* destination;

    bool multiPort;
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

#endif // PORT_H
