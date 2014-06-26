#include "datastreamobject.h"
#include <list>
#include "headerparser.h"

using namespace std;

DatastreamObject::DatastreamObject(string name, DataTypeStruct *type) :
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

std::list<PortOut*> DatastreamObject::getOutPorts(port_type type) {
    std::list<PortOut*> res;
    if (type == PORT_TYPE_CONTROL_OUT)
        res.insert(res.end(), controlOutPorts.begin(), controlOutPorts.end());
    else
        res.insert(res.end(), dataOutPorts.begin(), dataOutPorts.end());
    return res;
}

#include <iostream>
void DatastreamObject::findPorts() {
    DataType* controlPortType = DataType::getType("control_port_t");
    DataType* dataPortType = DataType::getType("data_port_t");
    DataType* voidType = DataType::getType("void");
    DataType* intType = DataType::getType("int");

    CMethod CInSig("cin_get_sig_dummy", new CMethodParameter("return", controlPortType, false));
    CInSig.addParameter(new CMethodParameter("this", type, true));
    CMethod DInSig("din_get_sig_dummy", new CMethodParameter("return", dataPortType, false));
    DInSig.addParameter(new CMethodParameter("this", type, true));
    CMethod COutSig("cout_set_sig_dummy", new CMethodParameter("return", voidType, false));
    COutSig.addParameter(new CMethodParameter("this", type, true));
    COutSig.addParameter(new CMethodParameter("port", controlPortType, true));
    CMethod DOutSig("dout_set_sig_dummy", new CMethodParameter("return", voidType, false));
    DOutSig.addParameter(new CMethodParameter("this", type, true));
    DOutSig.addParameter(new CMethodParameter("port", dataPortType, true));
    CMethod COutAddSig("cout_add_sig_dummy", new CMethodParameter("return", intType, false));
    COutAddSig.addParameter(new CMethodParameter("this", type, true));
    COutAddSig.addParameter(new CMethodParameter("port", controlPortType, true));
    CMethod DOutAddSig("dout_add_sig_dummy", new CMethodParameter("return", intType, false));
    DOutAddSig.addParameter(new CMethodParameter("this", type, true));
    DOutAddSig.addParameter(new CMethodParameter("port", dataPortType, true));

    list<CMethod*> methods = type->getMethods();
    for (list<CMethod*>::iterator i = methods.begin(); i != methods.end(); i++) {
        CMethod* m = *i;
        string port_name = m->getName();
        port_name.erase(0, 4);
        if (m->sameSignature(DInSig)) {
            addPort(new DataPortIn(port_name));
        } else if (m->sameSignature(CInSig)) {
            addPort(new ControlPortIn(port_name));
        } else if (m->sameSignature(DOutSig)) {
            addPort(new DataPortOut(port_name));
        } else if (m->sameSignature(COutSig)) {
            addPort(new ControlPortOut(port_name));
        } else {
            string maxDef = type->getCleanedName().append("_").append(port_name).append("_MAX");
            transform(maxDef.begin(), maxDef.end(), maxDef.begin(), ::toupper);
            if (m->sameSignature(DOutAddSig)) {
                for (int i = 0; i < HeaderParser::getDefinedInteger(maxDef); i++) { //TODO use really definied number
                    addPort(new DataPortOut(port_name));
                }
            } else if (m->sameSignature(COutAddSig)) {
                for (int i = 0; i < HeaderParser::getDefinedInteger(maxDef); i++) { //TODO use really definied number
                    addPort(new ControlPortOut(port_name));
                }
            }
        }
    }
}

void DatastreamObject::addPort(ControlPortIn* port) {
    controlInPorts.push_back(port);
    connect(port, SIGNAL(connected()), this, SLOT(portConnected()));
    connect(port, SIGNAL(disconnected(Port*)), this, SLOT(portDisconnected(Port*)));
}

void DatastreamObject::addPort(ControlPortOut* port) {
    controlOutPorts.push_back(port);
    connect(port, SIGNAL(connected()), this, SLOT(portConnected()));
    connect(port, SIGNAL(disconnected(Port*)), this, SLOT(portDisconnected(Port*)));
}

void DatastreamObject::addPort(DataPortIn* port) {
    dataInPorts.push_back(port);
    connect(port, SIGNAL(connected()), this, SLOT(portConnected()));
    connect(port, SIGNAL(disconnected(Port*)), this, SLOT(portDisconnected(Port*)));
}

void DatastreamObject::addPort(DataPortOut* port) {
    dataOutPorts.push_back(port);
    connect(port, SIGNAL(connected()), this, SLOT(portConnected()));
    connect(port, SIGNAL(disconnected(Port*)), this, SLOT(portDisconnected(Port*)));
}

void DatastreamObject::portConnected() {
    emit connectionsChanged();
}

template <typename T>
bool DatastreamObject::reorderUnconnectedPort(std::list<T *>& ports, Port* port) {
    for (typename list<T*>::iterator i = ports.begin(); i != ports.end(); i++) {
        if (*i == port) {
            typename list<T*>::iterator element = i;
            while (++i != ports.end()) {
                if ((port->getName().compare((*i)->getName())) != 0) {
                    break;
                }
            }
            ports.splice(i, ports, element);
            return true;
        }
    }
    return false;
}

void DatastreamObject::portDisconnected(Port* port) {
    if (!reorderUnconnectedPort(controlOutPorts, port))
        reorderUnconnectedPort(dataOutPorts, port);
    emit connectionsChanged();
}
