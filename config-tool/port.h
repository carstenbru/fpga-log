#ifndef PORT_H
#define PORT_H

#include <QObject>
#include <string>

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
    Port(std::string name) : name(name) {}
    virtual ~Port() {}

    virtual int connectPort(Port* port) = 0;
    virtual void disconnectPort() {}
    std::string getName() { return name; }
private:
    std::string name;
signals:
    void connected();
    void disconnected();
};

class ControlPortIn : public Port
{
public:
    ControlPortIn( std::string name) : Port(name) {}
    virtual ~ControlPortIn() {}

    virtual int connectPort(Port *port);
};

class DataPortIn : public Port
{
public:
    DataPortIn( std::string name) : Port(name) {}
    virtual ~DataPortIn() {}

    virtual int connectPort(Port* port);
};

class PortOut : public Port
{
public:
    PortOut(std::string name) : Port(name), destination(NULL) {}
    virtual ~PortOut() {}
    Port* getDestination() { return destination; }
    virtual void disconnectPort();

protected:
    Port* destination;
};

class ControlPortOut : public PortOut
{
public:
    ControlPortOut( std::string name) : PortOut(name) {}
    virtual ~ControlPortOut() {}

    virtual int connectPort(Port* port);
};

class DataPortOut : public PortOut
{
public:
    DataPortOut( std::string name) : PortOut(name) {}
    virtual ~DataPortOut() {}

    virtual int connectPort(Port* port);
};

#endif // PORT_H
