#include "datastreamobject.h"

DatastreamObject::DatastreamObject(std::string type) :
    type(type),
    position(QPoint(0,0))
{

    addPort(new DataPortOut("dout1")); //TODO remove
    addPort(new ControlPortIn("cin1"));
    addPort(new DataPortIn("din1"));
    addPort(new ControlPortOut("cout1"));
}

DatastreamObject::~DatastreamObject() {
    for (std::list<ControlPortIn*>::iterator i = controlInPorts.begin(); i != controlInPorts.end(); i++) {
        delete *i;
    }
    for (std::list<ControlPortOut*>::iterator i = controlOutPorts.begin(); i != controlOutPorts.end(); i++) {
        delete *i;
    }
    for (std::list<DataPortIn*>::iterator i = dataInPorts.begin(); i != dataInPorts.end(); i++) {
        delete *i;
    }
    for (std::list<DataPortOut*>::iterator i = dataOutPorts.begin(); i != dataOutPorts.end(); i++) {
        delete *i;
    }
}

std::string DatastreamObject::getType() {
    return type;
}

std::list<PortOut*> DatastreamObject::getOutPorts(port_type type) {
    std::list<PortOut*> res;
    if (type == PORT_TYPE_CONTROL_OUT)
        res.insert(res.end(), controlOutPorts.begin(), controlOutPorts.end());
    else
        res.insert(res.end(), dataOutPorts.begin(), dataOutPorts.end());
    return res;
}

void DatastreamObject::addPort(ControlPortIn* port) {
    controlInPorts.push_back(port);
    connect(port, SIGNAL(connected()), this, SLOT(portConnectionChanged()));
    connect(port, SIGNAL(disconnected()), this, SLOT(portConnectionChanged()));
}

void DatastreamObject::addPort(ControlPortOut* port) {
    controlOutPorts.push_back(port);
    connect(port, SIGNAL(connected()), this, SLOT(portConnectionChanged()));
    connect(port, SIGNAL(disconnected()), this, SLOT(portConnectionChanged()));
}

void DatastreamObject::addPort(DataPortIn* port) {
    dataInPorts.push_back(port);
    connect(port, SIGNAL(connected()), this, SLOT(portConnectionChanged()));
    connect(port, SIGNAL(disconnected()), this, SLOT(portConnectionChanged()));
}

void DatastreamObject::addPort(DataPortOut* port) {
    dataOutPorts.push_back(port);
    connect(port, SIGNAL(connected()), this, SLOT(portConnectionChanged()));
    connect(port, SIGNAL(disconnected()), this, SLOT(portConnectionChanged()));
}

void DatastreamObject::portConnectionChanged() {
    emit connectionsChanged();
}
