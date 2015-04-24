#include "port.h"
#include "datastreamobject.h"

using namespace std;

int ControlPortIn::connectPort(Port *port) {
    ControlPortOut* d = dynamic_cast<ControlPortOut*>(port);
    if (d != NULL) {
        return d->connectPort(this);
    } else {
        return 0;
    }
}

int ControlPortOut::connectPort(Port *port) {
    ControlPortIn* d = dynamic_cast<ControlPortIn*>(port);
    if (d != NULL) {
        destination = d;
        connect(port, SIGNAL(disconnectFromDest()), this, SLOT(destDisconnected()));
        emit connected();
        return 1;
    } else {
        return 0;
    }
}

int DataPortIn::connectPort(Port *port) {
    DataPortOut* d = dynamic_cast<DataPortOut*>(port);
    if (d != NULL) {
        return d->connectPort(this);
    } else {
        return 0;
    }
}

int DataPortOut::connectPort(Port *port) {
    DataPortIn* d = dynamic_cast<DataPortIn*>(port);
    if (d != NULL) {
        destination = d;
        connect(port, SIGNAL(disconnectFromDest()), this, SLOT(destDisconnected()));
        emit connected();
        return 1;
    } else {
        return 0;
    }
}

void PortOut::disconnectPort() {
    destination = NULL;
    emit disconnected(this);
}

void PortOut::destDisconnected() {
    disconnect(destination, SIGNAL(disconnectFromDest()), this, SLOT(destDisconnected()));
    disconnectPort();
}

void Port::saveToXml(QXmlStreamWriter& out, std::string type) {
    out.writeStartElement(type.c_str());
    out.writeAttribute("name", name.c_str());
    out.writeEndElement();
}

void PortOut::saveToXml(QXmlStreamWriter& out, std::string type) {
    out.writeStartElement(type.c_str());
    out.writeAttribute("name", getName().c_str());
    out.writeAttribute("multiPort", multiPort ? "true" : "false");
    if (isConnected()) {
        out.writeAttribute("destinationModule", destination->getParent()->getName().c_str());
        out.writeAttribute("destinationPort", destination->getName().c_str());
    }
    out.writeEndElement();
}

void PortOut::load(QXmlStreamReader& in, map<PortOut*, stringPair>& connections) {
    destination = NULL;
    multiPort = (in.attributes().value("multiPort").toString().compare("true") == 0);

    string destModule = in.attributes().value("destinationModule").toString().toStdString();
    if (!destModule.empty()) {
        stringPair p;
        p.first = destModule;
        p.second = in.attributes().value("destinationPort").toString().toStdString();
        connections[this] = p;
    }

    in.skipCurrentElement();
}

QXmlStreamWriter& operator<<(QXmlStreamWriter& out, ControlPortIn& port) {
    port.saveToXml(out, "ControlPortIn");
    return out;
}

QXmlStreamWriter& operator<<(QXmlStreamWriter& out, ControlPortOut& port) {
    port.saveToXml(out, "ControlPortOut");
    return out;
}

QXmlStreamWriter& operator<<(QXmlStreamWriter& out, DataPortIn& port) {
    port.saveToXml(out, "DataPortIn");
    return out;
}

QXmlStreamWriter& operator<<(QXmlStreamWriter& out, DataPortOut& port) {
    port.saveToXml(out, "DataPortOut");
    return out;
}
