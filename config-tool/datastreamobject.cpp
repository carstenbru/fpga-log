#include "datastreamobject.h"
#include <list>

using namespace std;

DatastreamObject::DatastreamObject(string name, DataType *type) :
    CObject(name, type),
    position(QPoint(0,0))
{
    findPorts();
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

std::string DatastreamObject::getDisplayName() {
    return type->getDisplayName();
}

std::list<PortOut*> DatastreamObject::getOutPorts(port_type type) {
    std::list<PortOut*> res;
    if (type == PORT_TYPE_CONTROL_OUT)
        res.insert(res.end(), controlOutPorts.begin(), controlOutPorts.end());
    else
        res.insert(res.end(), dataOutPorts.begin(), dataOutPorts.end());
    return res;
}

void DatastreamObject::findPorts() {
    DataType* controlPortType = DataType::getType("control_port_t");
    DataType* dataPortType = DataType::getType("data_port_t");
    DataType* voidType = DataType::getType("void");

    CMethod CInSig("cin_sig_dummy", new CMethodParameter("return", controlPortType, false));
    CInSig.addParameter(new CMethodParameter("this", type, true));
    CMethod DInSig("din_sig_dummy", new CMethodParameter("return", dataPortType, false));
    DInSig.addParameter(new CMethodParameter("this", type, true));
    CMethod COutSig("cout_sig_dummy", new CMethodParameter("return", voidType, false));
    COutSig.addParameter(new CMethodParameter("this", type, true));
    COutSig.addParameter(new CMethodParameter("port", controlPortType, true));
    CMethod DOutSig("cout_sig_dummy", new CMethodParameter("return", voidType, false));
    DOutSig.addParameter(new CMethodParameter("this", type, true));
    DOutSig.addParameter(new CMethodParameter("port", dataPortType, true));

    list<CMethod*> methods = type->getMethods();
    for (list<CMethod*>::iterator i = methods.begin(); i != methods.end(); i++) {
        CMethod* m = *i;
        string port_name = m->getName();
        port_name.erase(0, 4);
        if (m->sameSignature(DInSig)) {
            addPort(new DataPortIn(port_name));
        } else
        if (m->sameSignature(CInSig)) {
            addPort(new ControlPortIn(port_name));
        } else
        if (m->sameSignature(DOutSig)) {
            addPort(new DataPortOut(port_name));
        } else
        if (m->sameSignature(COutSig)) {
            addPort(new ControlPortOut(port_name));
        }
    }
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
