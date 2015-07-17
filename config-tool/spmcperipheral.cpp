#include "spmcperipheral.h"
#include <QFile>
#include <iostream>
#include <QProcessEnvironment>
#include <QDirIterator>
#include "datalogger.h"
#include "cobject.h"

using namespace std;

std::map<std::string, std::string> SpmcPeripheral::peripheralXMLs;

SpmcPeripheral::SpmcPeripheral(string name, DataType *type, CObject* parentObject, DataLogger* dataLogger) :
    name(name),
    dataLogger(dataLogger),
    dataType(type),
    parentObject(parentObject)
{
    readPeripheralXML();
    readModuleXML();
}

SpmcPeripheral::SpmcPeripheral(QXmlStreamReader& in, CObject* parentObject, DataLogger *dataLogger) :
    SpmcPeripheral(in.attributes().value("name").toString().toStdString(),
                   DataType::getType(in.attributes().value("dataType").toString().toStdString()),
                   parentObject,
                   dataLogger)
{
    unsigned int paramCount = 0;
    unsigned int portGroupsCount = 0;
    bool paramsChanged = false;

    while (in.readNextStartElement()) {
        if (in.name().compare("parameter") == 0) {
            paramCount++;
            CParameter* param = new CParameter(in);
            if (param != NULL) {
                if (!setParameter(param)) {
                    paramsChanged = true;
                    delete param;
                }
            }
        } else if (in.name().compare("portGroup") == 0) {
            string name = in.attributes().value("name").toString().toStdString();
            try {
                list<PeripheralPort*> portGroup = ports.at(name);
                portGroupsCount++;
                unsigned int portsCount = 0;
                while (in.readNextStartElement()) {
                    if (in.name().compare("peripheralPort") == 0) {
                        portsCount++;
                        string name = in.attributes().value("name").toString().toStdString();
                        for (list<PeripheralPort*>::iterator i = portGroup.begin(); i != portGroup.end(); i++) {
                            if ((*i)->getName().compare(name) == 0) {
                                (*i)->load(in, parentObject);

                                string ref = (*i)->getWidthRef();
                                if (!ref.empty()) {
                                    CParameter* p = getParameter(ref);
                                    p->setCritical(true);
                                    QObject::connect(p, SIGNAL(valueChanged(std::string)), (*i), SLOT(newWidth(std::string)));
                                    (*i)->newWidth(p->getValue());
                                }

                                break;
                            }
                        }
                    } else
                        in.skipCurrentElement();
                }
                ports[name] = portGroup;
                if (portsCount != portGroup.size())
                    paramsChanged = true;
            } catch (exception e) {
                paramsChanged = true;
            }
        }
    }
    if (portGroupsCount != ports.size())
        paramsChanged = true;
    if (paramCount != parameters.size()) {
        paramsChanged = true;
    }
    if (paramsChanged) {
        parentObject->setDefinitionsUpdated(true);
    }
}

SpmcPeripheral::~SpmcPeripheral() {
    for (list<CParameter*>::iterator i = parameters.begin(); i != parameters.end(); i++) {
        delete *i;
    }
    for (map<string, list<PeripheralPort*> >::iterator mi = ports.begin(); mi != ports.end(); mi++) {
        for (list<PeripheralPort*>::iterator i = mi->second.begin(); i != mi->second.end(); i++) {
            delete *i;
        }
    }
}

bool SpmcPeripheral::setParameter(CParameter* newValue) {
    for (list<CParameter*>::iterator i = parameters.begin(); i != parameters.end(); i++) {
        if ((*i)->sameSignature(*newValue) && ((*i)->getName().compare(newValue->getName()) == 0)) {
            delete *i;
            *i = newValue;
            return true;
        }
    }
    return false;
}

CParameter* SpmcPeripheral::getParameter(std::string name) {
    for (list<CParameter*>::iterator i = parameters.begin(); i != parameters.end(); i++) {
        if ((*i)->getName().compare(name) == 0)
            return *i;
    }
    return NULL;
}

PeripheralPort* SpmcPeripheral::getPort(std::string group, std::string name) {
    list<PeripheralPort*> portGroup = ports.at(group);
    for (list<PeripheralPort*>::iterator i = portGroup.begin(); i != portGroup.end(); i++) {
        if ((*i)->getName().compare(name) == 0)
            return *i;
    }
    return NULL;
}

std::string SpmcPeripheral::readModuleNameFromFile(std::string fileName) {
    QFile file(QString(fileName.c_str()));
    if (!file.open(QIODevice::ReadOnly)) {
        cerr << "unable to open peripheral module xml file: " << fileName << endl;
        return "unable to open";
    }
    QXmlStreamReader reader(&file);
    reader.readNextStartElement();

    //in newer spmc versions we need to look in the header tag for the peripheral name and not in the first one
    if (reader.name().compare("peripheral") == 0) {
        reader.readNextStartElement();
    }

    return reader.attributes().value("name").toString().toStdString();
}

void SpmcPeripheral::loadPeripheralXMLs() {
    string path = QProcessEnvironment::systemEnvironment().value("SPARTANMC_ROOT").toStdString();
    path += "/spartanmc/hardware/";
    QDirIterator dirIter(QString(path.c_str()), (QDirIterator::Subdirectories | QDirIterator::FollowSymlinks));
    while (dirIter.hasNext()) {
        dirIter.next();
        if (QFileInfo(dirIter.filePath()).isFile()) {
            if (dirIter.fileName() == "module.xml") {
                string fileName = dirIter.filePath().toStdString();
                peripheralXMLs[readModuleNameFromFile(fileName)] = fileName;
            }
        }
    }
}

string SpmcPeripheral::getFileName() {
    string periName = dataType->getName();
    periName.erase(periName.length() - 7, periName.length());
    try {
        return peripheralXMLs.at(periName);
    } catch (exception) {
        cerr << "unable to find peripheral module xml file: " << dataType->getName() << endl;
        return "not found";
    }
}

void SpmcPeripheral::readParameterElement(QXmlStreamReader& reader) {
    QXmlStreamAttributes attributes = reader.attributes();
    string paramName = attributes.value("name").toString().toStdString();
    string type = attributes.value("type").toString().toStdString();
    if (type.compare("int") == 0)
        type = "peripheral_int";
    DataTypeEnumeration* dt = NULL;
    while (reader.readNextStartElement()) {
        if (reader.name().compare("choice") == 0) {
            type = dataType->getName() + "_" + paramName;
            if (reader.name().toString().compare("range") == 0) {
                try {
                    DataType::getType(type);
                } catch (exception) {
                    QXmlStreamAttributes attributes = reader.attributes();
                    int min = attributes.value("min").toString().toInt();
                    int max = attributes.value("max").toString().toInt();
                    new DataTypeNumber(type, min, max, true);
                }
            } else if (reader.name().toString().compare("choice") == 0) {
                if (dt == NULL) {
                    try {
                        DataType::getType(type);
                    } catch (exception) {
                        dt = new DataTypeEnumeration(type, true);
                        dt->addValue(reader.attributes().value("value").toString().toStdString());
                    }
                } else
                    dt->addValue(reader.attributes().value("value").toString().toStdString());
            }
        }
        reader.skipCurrentElement();
    }
    string value = attributes.value("value").toString().toStdString();
    CParameter* param = new CParameter(paramName, DataType::getType(type), false, value);
    parameters.push_back(param);
    if (paramName.compare("CLOCK_FREQUENCY") == 0)
        param->setHideFromUser(true);
}

void SpmcPeripheral::readPortsElement(QXmlStreamReader& reader, std::string direction) {
    string groupName = "global";
    QStringRef group = reader.attributes().value("group");
    if (!group.isEmpty()) {
        groupName = group.toString().toStdString();
    }
    while (reader.readNextStartElement()) {
        if (reader.name().compare("port") == 0) {
            PeripheralPort* port;
            string portName = reader.attributes().value("name").toString().toStdString();
            QStringRef width = reader.attributes().value("width");
            if (width.isEmpty()) {
                port = new PeripheralPort(portName);
            } else {
                bool directValue;
                int portWidth = width.toString().toInt(&directValue);
                if (!directValue) {
                    string symbol = width.toString().toStdString();
                    symbol.erase(0,2);
                    symbol.erase(symbol.length() - 1, symbol.length());
                    CParameter* p = getParameter(symbol);
                    p->setCritical(true);
                    port = new PeripheralPort(portName,  p);
                    QObject::connect(p, SIGNAL(valueChanged(std::string)), port, SLOT(newWidth(std::string)));
                } else
                    port = new PeripheralPort(portName, portWidth);
            }
            string pinDiretion = reader.attributes().value("direction").toString().toStdString();
            if (pinDiretion.empty())
                port->setDirection(direction);
            else
                port->setDirection(pinDiretion);
            ports[groupName].push_back(port);
        }
        reader.skipCurrentElement();
    }
}

void SpmcPeripheral::readPeripheralXML() {
    QFile file(QString(getFileName().c_str()));
    if (!file.open(QIODevice::ReadOnly)) {
        cerr << "unable to open peripheral module xml file: " << dataType->getName() << endl;
        return;
    }
    QXmlStreamReader reader(&file);
    reader.readNextStartElement();

    while (reader.readNextStartElement()) {
        if (reader.name().compare("parameters") == 0) {
            while (reader.readNextStartElement())
                readParameterElement(reader);
        } else if (reader.name().compare("ports") == 0) {
            string diretion = reader.attributes().value("direction").toString().toStdString();
            readPortsElement(reader, diretion);
        } else
            reader.skipCurrentElement();
    }
}

void SpmcPeripheral::readModuleXML() {
    string moduleXmlFile = "../config-tool-files/modules/";
    moduleXmlFile += parentObject->getType()->getCleanedName();
    moduleXmlFile += ".xml";
    QFile file(QString(moduleXmlFile.c_str()));
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }
    QXmlStreamReader reader(&file);
    reader.readNextStartElement();
    while (reader.readNextStartElement()) {
        if ((reader.name().toString().compare("peripheral") == 0) &&
                (reader.attributes().value("name").toString().toStdString().compare(name) == 0)) {
            while (reader.readNextStartElement()) {
                if (reader.name().toString().compare("parameter") == 0) {
                    QXmlStreamAttributes attributes = reader.attributes();
                    CParameter* parameter = getParameter(attributes.value("name").toString().toStdString());
                    QStringRef value = attributes.value("value");
                    if (parameter != NULL) {
                        if (!value.isEmpty()) {
                            parameter->setValue(value.toString().toStdString());
                        }
                        QStringRef hideStr = attributes.value("hide");
                        if (!hideStr.isEmpty()) {
                            bool hide = (hideStr.toString().compare("TRUE") == 0);
                            parameter->setHideFromUser(hide);
                        }
                    }
                } else if (reader.name().toString().compare("port") == 0) {
                    QXmlStreamAttributes attributes = reader.attributes();
                    PeripheralPort* port = getPort(attributes.value("group").toString().toStdString(), attributes.value("name").toString().toStdString());
                    QStringRef destination = attributes.value("destination");
                    if (port != NULL) {
                        if (!destination.isEmpty()) {
                            CParameter* p = port->getLines().front();
                            p->setValue(destination.toString().toStdString());

                            QStringRef parameter = attributes.value("parameter");
                            if (!parameter.isEmpty()) {
                                string paramStr = parameter.toString().toStdString();
                                p->setName(paramStr);
                                parentObject->getInitMethod()->getParameter(paramStr)->setHideFromUser(true);
                            }
                        }
                        QStringRef hideStr = attributes.value("hide");
                        if (!hideStr.isEmpty()) {
                            bool hide = (hideStr.toString().compare("TRUE") == 0);
                            port->setHideFromUser(hide);
                        }
                        QStringRef constraints = attributes.value("constraints");
                        if (!constraints.isEmpty()) {
                            port->setConstraints(constraints.toString().toStdString());
                        }
                    }
                }
                reader.skipCurrentElement();
            }
        } else
            reader.skipCurrentElement();
    }
}

std::string SpmcPeripheral::getCompleteName() {
    return parentObject->getName() + "_" + name;
}

std::string SpmcPeripheral::getParentName() {
    return parentObject->getName();
}

void SpmcPeripheral::saveToXml(QXmlStreamWriter& out) {
    out.writeStartElement("SpmcPeripheral");

    out.writeAttribute("name", name.c_str());
    out.writeAttribute("dataType", dataType->getName().c_str());
    for (list<CParameter*>::iterator i = parameters.begin(); i != parameters.end(); i++) {
        out << **i;
    }
    for (map<string, list<PeripheralPort*> >::iterator i = ports.begin(); i != ports.end(); i++) {
        out.writeStartElement("portGroup");

        out.writeAttribute("name", i->first.c_str());
        for (list<PeripheralPort*>::iterator portsIt = i->second.begin(); portsIt != i->second.end(); portsIt++) {
            out << **portsIt;
        }

        out.writeEndElement();
    }

    out.writeEndElement();
}

QXmlStreamWriter& operator<<(QXmlStreamWriter& out, SpmcPeripheral& peripheral) {
    peripheral.saveToXml(out);
    return out;
}

PeripheralPort::PeripheralPort(std::string name) :
    name(name)
{
    lines.push_back(new CParameter(name, DataTypePin::getPinType(), false));
}

PeripheralPort::PeripheralPort(std::string name, CParameter* widthRef) :
    PeripheralPort(name, atoi(widthRef->getValue().c_str()))
{
    this->widthRef = widthRef->getName();
}

PeripheralPort::PeripheralPort(std::string name, int width) :
    name(name)
{
    for (int i = 0; i < width; i++) {
        lines.push_back(new CParameter(name + "_" + to_string(i), DataTypePin::getPinType(), false));
    }
}

bool PeripheralPort::setLine(CParameter* newValue) {
    for (list<CParameter*>::iterator i = lines.begin(); i != lines.end(); i++) {
        if ((*i)->sameSignature(*newValue) && (newValue->getName().compare((*i)->getName()) == 0)) {
            i = lines.erase(i);
            lines.insert(i, newValue);
            return true;
        }
    }
    return false;
}

void PeripheralPort::load(QXmlStreamReader& in, CObject* parent) {
    bool changed = false;

    if (name.compare(in.attributes().value("name").toString().toStdString()) != 0)
        changed = true;
    if (direction.compare(in.attributes().value("direction").toString().toStdString()) != 0)
        changed = true;

    if (widthRef.compare(in.attributes().value("widthRef").toString().toStdString()) != 0)
        changed = true;
    if (constraints.compare(in.attributes().value("constraints").toString().toStdString()) != 0)
        changed = true;

    while (in.readNextStartElement()) {
        CParameter* param = new CParameter(in);
        if (!setLine(param)) {
            changed = true;
            delete param;
        }
    }

    if (changed)
        parent->setDefinitionsUpdated(true);
}

PeripheralPort::~PeripheralPort() {
    for (list<CParameter*>::iterator i = lines.begin(); i != lines.end(); i++) {
        delete *i;
    }
}

void PeripheralPort::setHideFromUser(bool hide) {
    for (list<CParameter*>::iterator i = lines.begin(); i != lines.end(); i++) {
        (*i)->setHideFromUser(hide);
    }
}

void PeripheralPort::setDirection(std::string direction) {
    transform(direction.begin(), direction.end(), direction.begin(), ::toupper);
    this->direction = direction;
}

void PeripheralPort::newWidth(std::string widthVal) {
    int newWidth = atoi(widthVal.c_str());
    int width = lines.size();

    if (newWidth < width) {
        int i = width - newWidth;
        while (i--) {
            CParameter* p =  lines.back();
            lines.pop_back();
            p->deleteLater();
        }
    }
    if (newWidth > width) {
        for (int i = width; i < newWidth; i++) {
            lines.push_back(new CParameter(name + "_" + to_string(i), DataTypePin::getPinType(), false));
        }
    }
    width = newWidth;
}

QXmlStreamWriter& operator<<(QXmlStreamWriter& out, PeripheralPort& port) {
    out.writeStartElement("peripheralPort");

    out.writeAttribute("name", port.name.c_str());
    out.writeAttribute("direction", port.direction.c_str());
    if (!port.widthRef.empty())
        out.writeAttribute("widthRef", port.widthRef.c_str());
    if (!port.constraints.empty())
        out.writeAttribute("constraints", port.constraints.c_str());

    for (list<CParameter*>::iterator i = port.lines.begin(); i != port.lines.end(); i++) {
        out << **i;
    }

    out.writeEndElement();

    return out;
}
