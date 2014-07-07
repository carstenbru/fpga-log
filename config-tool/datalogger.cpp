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
    clockFreq("Taktfrequenz", DataType::getType("peripheral_int"), false, "32000000")
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

                    pinType->addPin(groupName, pin);
                }
                reader.skipCurrentElement();
            }
        } else
            reader.skipCurrentElement();
    }
}

void DataLogger::newObject(DataTypeStruct *type) {
    if (type->hasPrefix("device_") || type->hasPrefix("dm_") || type->hasPrefix("sink_")) {
        DatastreamObject* dso = new DatastreamObject(findObjectName(datastreamObjects, type), type, this);
        datastreamObjects.push_back(dso);
        connect(dso, SIGNAL(connectionsChanged()), this, SLOT(moduleConnectionsChanged()));
        emit datastreamModulesChanged();
    }
    else {
        CObject* co = new CObject(findObjectName(otherObjects, type), type, this);
        otherObjects.push_back(co);
        emit otherModulesChanged();
    }
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
    //emit datastreamModulesChanged(); //TODO seg fault with this signal in mouse move event of MoveableButton
    emit otherModulesChanged();
    object->setName(newName);
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
    DataTypeEnumeration* targetType = new DataTypeEnumeration("target");

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

int DataLogger::getClk() {
    return atoi(clockFreq.getValue().c_str());
}

int DataLogger::getPeriClk() {
    return getClk() / 2;
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
        p->connectPort(dest);
    }

    emit dataLogger.datastreamModulesChanged();
    emit dataLogger.otherModulesChanged();
    emit dataLogger.connectionsChanged();

    return in;
}
