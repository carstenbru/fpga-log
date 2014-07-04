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
        type = dataType->getName() + "_" + paramName;
        if (reader.name().toString().compare("range") == 0) {
            try {
                DataType::getType(type);
            } catch (exception) {
                QXmlStreamAttributes attributes = reader.attributes();
                int min = attributes.value("min").toString().toInt();
                int max = attributes.value("max").toString().toInt();
                new DataTypeNumber(type, min, max);
            }
        } else if (reader.name().toString().compare("choice") == 0) {
            if (dt == NULL) {
                try {
                    DataType::getType(type);
                } catch (exception) {
                    dt = new DataTypeEnumeration(type);
                    dt->addValue(reader.attributes().value("value").toString().toStdString());
                }
            } else
                dt->addValue(reader.attributes().value("value").toString().toStdString());
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
                    port = new PeripheralPort(portName,  atoi(p->getValue().c_str()));
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
                    if (!value.isEmpty()) {
                        parameter->setValue(value.toString().toStdString());
                    }
                    QStringRef hideStr = attributes.value("hide");
                    if (!hideStr.isEmpty()) {
                        bool hide = (hideStr.toString().compare("TRUE") == 0);
                        parameter->setHideFromUser(hide);
                    }
                } else if (reader.name().toString().compare("port") == 0) {
                    QXmlStreamAttributes attributes = reader.attributes();
                    PeripheralPort* port = getPort(attributes.value("group").toString().toStdString(), attributes.value("name").toString().toStdString());
                    QStringRef destination = attributes.value("destination");
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

PeripheralPort::PeripheralPort(std::string name) :
    name(name),
    width(1)
{
    lines.push_back(new CParameter(name, DataTypePin::getPinType(), false));
}

PeripheralPort::PeripheralPort(std::string name, int width) :
    name(name),
    width(width)
{
    for (int i = 0; i < width; i++) {
        lines.push_back(new CParameter(name + "_" + to_string(i), DataTypePin::getPinType(), false));
    }
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
