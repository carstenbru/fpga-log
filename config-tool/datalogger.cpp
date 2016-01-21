#include "datalogger.h"
#include <sstream>
#include <iostream>
#include <QProcessEnvironment>
#include <QFile>
#include <QXmlStreamReader>
#include <QDirIterator>

using namespace std;

std::map<std::string, std::string> DataLogger::targetXMLs;

DataLogger::DataLogger() :
    target("Zielplattform", DataType::getType("target"), false, "papilio-pro"),
    clockPin("Clock Pin", DataType::getType("pin"), false, "CLK:32"),
    clockFreq("Taktfrequenz", DataType::getType("peripheral_int"), false, "32000000"),
    definitionsUpdated(false)
{
    loadTargetPins();
}

DataLogger::~DataLogger() {
    std::list<DatastreamObject*>::iterator i;
    for (i = datastreamObjects.begin(); i != datastreamObjects.end(); i++) {
        delete *i;
    }
}

void DataLogger::loadTargetPins() {
    DataTypePin* pinType = DataTypePin::getPinType();
    pinType->clear();

    QFile file(QString(targetXMLs[target.getValue()].c_str()));
    if (!file.open(QIODevice::ReadOnly)) {
        cerr << "unable to open target xml file: " << target.getValue() << endl;
        return;
    }
    QXmlStreamReader reader(&file);
    reader.readNextStartElement();
    while (reader.readNextStartElement()) {
        if (reader.name().compare("iopins") == 0) {
            string groupName = reader.attributes().value("name").toString().toStdString();
            while (reader.readNextStartElement()) {
                if (reader.name().compare("pin") == 0) {
                    Pin pin(reader.attributes().value("name").toString().toStdString());

                    QXmlStreamAttributes attributes = reader.attributes();
                    QStringRef freq = attributes.value("freq");
                    if (!freq.isEmpty()) {
                        pin.setFreq(freq.toString().toStdString());
                    }

                    QStringRef loc = attributes.value("fpgapin");
                    if (!loc.isEmpty()) {
                        pin.setLoc(loc.toString().toStdString());
                    }

                    pinType->addPin(groupName, pin);
                }
                reader.skipCurrentElement();
            }
        } else
            reader.skipCurrentElement();
    }
}

string DataLogger::newObject(DataTypeStruct *type) {
    string name;
    if (type->hasPrefix("device_") || type->hasPrefix("dm_") || type->hasPrefix("sink_")) {
        name = findObjectName(datastreamObjects, type);
        DatastreamObject* dso = new DatastreamObject(name, type, this);
        datastreamObjects.push_back(dso);
        connect(dso, SIGNAL(connectionsChanged()), this, SLOT(moduleConnectionsChanged()));
        emit datastreamModulesChanged();
    }
    else {
        name = findObjectName(otherObjects, type);
        CObject* co = new CObject(name, type, this);
        otherObjects.push_back(co);
        emit otherModulesChanged();
    }
    return name;
}

template <typename T>
bool DataLogger::deleteObject(T& searchList, CObject *object) {
    for (typename T::iterator i = searchList.begin(); i != searchList.end(); i++) {
        if ((*i) == object) {
            delete *i;
            i = searchList.erase(i);
            return true;
        }
    }
    return false;
}

void DataLogger::deleteObject(CObject *object) {
    if (deleteObject(datastreamObjects, object))
        emit datastreamModulesChanged();
    else if (deleteObject(otherObjects, object))
        emit otherModulesChanged();
}

void DataLogger::moduleConnectionsChanged() {
    emit connectionsChanged();
}

template <typename T>
bool DataLogger::containsObjectName(T searchList, std::string name) {
    for (typename T::iterator i = searchList.begin(); i != searchList.end(); i++) {
        if ((*i)->getName().compare(name) == 0)
            return true;
    }
    return false;
}

template <typename T>
std::string DataLogger::findObjectName(T searchList, DataType* dataType) {
    int i = 0;
    stringstream ss;
    do {
        ss.str(string());
        ss << dataType->getDisplayName();
        ss << "_";
        ss << i;
        i++;
    } while (containsObjectName(searchList, ss.str()));

    return ss.str();
}

bool DataLogger::changeObjectName(CObject* object, std::string newName) {
    if (newName.empty()) {
        cerr << "leerer Modulname ist nicht erlaubt" << endl;
        return false;
    }
    if (containsObjectName(datastreamObjects, newName) || containsObjectName(otherObjects, newName)) {
        cerr << "Modulname bereits vergeben" << endl;
        return false;
    }
    object->setName(newName);

    emit datastreamModulesChanged();
    emit otherModulesChanged();
    return true;
}

template <typename T>
void DataLogger::addInstancesToList(T searchList, std::list<CObject*>& destList, DataTypeStruct* dataType) {
    for (typename T::iterator i = searchList.begin(); i != searchList.end(); i++) {
        if ((*i)->getType()->isSuperclass(dataType))
            destList.push_back(*i);
    }
}

std::list<CObject*> DataLogger::getInstances(DataTypeStruct* dataType) {
    std::list<CObject*> res;
    addInstancesToList(datastreamObjects, res, dataType);
    addInstancesToList(otherObjects, res, dataType);
    return res;
}

std::map<std::string, CObject*> DataLogger::getObjectsMap() {
    map<string, CObject*> map;
    for (list<DatastreamObject*>::iterator i = datastreamObjects.begin(); i != datastreamObjects.end(); i++) {
        map[(*i)->getName()] = (*i);
    }
    for (vector<CObject*>::iterator i = otherObjects.begin(); i != otherObjects.end(); i++) {
        map[(*i)->getName()] = (*i);
    }
    return map;
}

string DataLogger::readTargetNameFromFile(string fileName) {
    QFile file(QString(fileName.c_str()));
    if (!file.open(QIODevice::ReadOnly)) {
        cerr << "unable to open target xml file: " << fileName << endl;
        return "unable to open";
    }
    QXmlStreamReader reader(&file);
    reader.readNextStartElement();
    return reader.attributes().value("name").toString().toStdString();
}

void DataLogger::loadTragetXMLs() {
    DataTypeEnumeration* targetType = new DataTypeEnumeration("target", true);

    string path = QProcessEnvironment::systemEnvironment().value("SPARTANMC_ROOT").toStdString();
    path += "/lib/targets/";
    QDirIterator dirIter(QString(path.c_str()), (QDirIterator::Subdirectories | QDirIterator::FollowSymlinks));
    while (dirIter.hasNext()) {
        dirIter.next();
        if (QFileInfo(dirIter.filePath()).isFile()) {
            if (QFileInfo(dirIter.filePath()).suffix() == "xml") {
                string fileName = dirIter.filePath().toStdString();
                string targetName = readTargetNameFromFile(fileName);
                targetXMLs[targetName] = fileName;
                targetType->addValue(targetName);

            }
        }
    }
}

void DataLogger::parameterChanged() {
    emit criticalParameterChanged();
}

void DataLogger::pinParamterChanged() {
    emit storePins();
    emit pinChanged();
}

int DataLogger::getClk() {
    return atoi(clockFreq.getValue().c_str());
}

int DataLogger::getPeriClk() {
    return getClk() / 2;
}

map<CParameter*, string[4]> DataLogger::getPinAssignments() {
    map<CParameter*, string[4]> result;

    map<string, CObject*> objects = getObjectsMap();
    for (map<string, CObject*>::iterator oi = objects.begin(); oi != objects.end(); oi++) {
        list<SpmcPeripheral*> peripherals = oi->second->getPeripherals();
        for (list<SpmcPeripheral*>::iterator pi = peripherals.begin(); pi != peripherals.end(); pi++) {
            map<string, list<PeripheralPort*> > ports = (*pi)->getPorts();
            for (map<string, list<PeripheralPort*> >::iterator poi = ports.begin(); poi != ports.end(); poi++) {
                list<PeripheralPort*> pins = poi->second;
                for (list<PeripheralPort*>::iterator i = pins.begin(); i != pins.end(); i++) {
                    list<CParameter*> lines = (*i)->getLines();
                    for (std::list<CParameter*>::iterator li = lines.begin(); li != lines.end(); li++) {
                        if (!(*li)->getHideFromUser()) {
                            string* item = (string*)&result[*li];
                            item[0] = (*li)->getValue();
                            item[1] = oi->second->getName();
                            item[2] = poi->first;
                            item[3] = (*li)->getName();
                        }
                    }
                }
            }
        }
        //timestamp pins
        list<CParameter*> timestampPins = oi->second->getTimestampPinParameters();
        for (list<CParameter*>::iterator tpi = timestampPins.begin(); tpi != timestampPins.end(); tpi++) {
            string* item = (string*)&result[*tpi];
            item[0] = (*tpi)->getValue();
            item[1] = oi->second->getName();
            item[2] = "timestamp pin";
            item[3] = (*tpi)->getName();
        }
    }

    //clk pin
    string* clk_pin = (string*)&result[getClockPin()];
    clk_pin[0] = getClockPin()->getValue();
    clk_pin[1] = "SYSTEM";
    clk_pin[2] = "clock";
    clk_pin[3] = "in";
    return result;
}

QXmlStreamWriter& operator<<(QXmlStreamWriter& out, DataLogger& dataLogger) {
    out.writeStartElement("datalogger");

    out << dataLogger.target << dataLogger.clockPin << dataLogger.clockFreq;

    for (list<DatastreamObject*>::iterator i = dataLogger.datastreamObjects.begin(); i != dataLogger.datastreamObjects.end(); i++) {
        out << **i;
    }
    for (vector<CObject*>::iterator i = dataLogger.otherObjects.begin(); i != dataLogger.otherObjects.end(); i++) {
        out << **i;
    }

    out.writeEndElement();
    return out;
}

DatastreamObject* DataLogger::getDatastreamObject(std::string name) {
    for (list<DatastreamObject*>::iterator i = datastreamObjects.begin(); i != datastreamObjects.end(); i++) {
        if ((*i)->getName().compare(name) == 0)
            return *i;
    }
    return NULL;
}

QXmlStreamReader& operator>>(QXmlStreamReader& in, DataLogger& dataLogger) {
    in.readNextStartElement();

    if (in.name().compare("datalogger") != 0) {
        cerr << "Fehler: Datei ist kein fpga-log Datenlogger!" << endl;
        return in;
    }

    in >> dataLogger.target >> dataLogger.clockPin >> dataLogger.clockFreq;
    dataLogger.loadTargetPins();

    map<PortOut*, stringPair> connections;

    while (in.readNextStartElement()) {
        if (in.name().compare("DatastreamObject") == 0) {
            DatastreamObject* dso = new DatastreamObject(in, &dataLogger, connections);
            dataLogger.datastreamObjects.push_back(dso);
            QObject::connect(dso, SIGNAL(connectionsChanged()), &dataLogger, SLOT(moduleConnectionsChanged()));
        } else if (in.name().compare("CObject") == 0) {
            dataLogger.otherObjects.push_back(new CObject(in, &dataLogger));
        } else
            in.skipCurrentElement();
    }

    for (map<PortOut*, stringPair>::iterator i = connections.begin(); i != connections.end(); i++) {
        PortOut* p = i->first;
        Port* dest = dataLogger.getDatastreamObject(i->second.first)->getPort(i->second.second);
        if (dest != NULL)
            p->connectPort(dest);
    }

    emit dataLogger.datastreamModulesChanged();
    emit dataLogger.otherModulesChanged();
    emit dataLogger.connectionsChanged();

    return in;
}
