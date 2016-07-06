/**
 * @file datalogger.cpp
 * @brief DataLogger class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

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
    clockDivide("clock divide", DataType::getType("sysclk_regs_t_CLKFX_DIVIDE"), false, "4"),
    clockMultiply("clock multiply", DataType::getType("sysclk_regs_t_CLKFX_MULTIPLY"), false, "2"),
    expertMode("expert mode", DataType::getType("bool"), false, "FALSE"),
    definitionsUpdated(false)
{    
    loadTargetPins();
    connect(&expertMode, SIGNAL(valueChanged(std::string)), this, SLOT(expertModeParamChanged(std::string)));
}

DataLogger::DataLogger(DataLogger& other) : QObject() {
    //TODO better implementation
    QString loggerTmp;
    QXmlStreamWriter writer(&loggerTmp);
    writer << other;
    QXmlStreamReader reader(loggerTmp);
    reader >> *this;
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
        connect(dso, SIGNAL(viasChanged()), this, SLOT(viaChanged()));
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

void DataLogger::addDataStreamObject(DatastreamObject *dso) {
    datastreamObjects.push_back(dso);
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

void DataLogger::viaChanged() {
    emit viasChanged();
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

string DataLogger::findObjectName(bool isDataStreamObject, DataType* dataType) {
    if (isDataStreamObject) {
        return findObjectName(datastreamObjects, dataType);
    } else {
        return findObjectName(otherObjects, dataType);
    }
}

bool DataLogger::changeObjectName(CObject* object, std::string newName) {
    if (newName.empty()) {
        cerr << tr("empty module name not allowed").toStdString() << endl;
        return false;
    }
    if (containsObjectName(datastreamObjects, newName) || containsObjectName(otherObjects, newName)) {
        cerr << tr("module name already used").toStdString() << endl;
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
        cerr << tr("unable to open target xml file:").toStdString() << " " << fileName << endl;
        return tr("unable to open").toStdString();
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

int DataLogger::getClkDivide() {
    return atoi(clockDivide.getValue().c_str());
}

int DataLogger::getClkMultiply() {
    return atoi(clockMultiply.getValue().c_str());
}

int DataLogger::getSystemClk() {
    return getClk() / getClkDivide() * getClkMultiply();
}

int DataLogger::getPeriClk() {
    return getSystemClk();
}

map<CParameter*, string[5]> DataLogger::getPinAssignments() {
    map<CParameter*, string[5]> result;

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
                            item[4] = (*i)->getDirection();
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
            item[4] = "INPUT";
        }
    }

    //clk pin
    string* clk_pin = (string*)&result[getClockPin()];
    clk_pin[0] = getClockPin()->getValue();
    clk_pin[1] = "SYSTEM";
    clk_pin[2] = "clock";
    clk_pin[3] = "in";
    clk_pin[4] = "INPUT";
    return result;
}

QXmlStreamWriter& operator<<(QXmlStreamWriter& out, DataLogger& dataLogger) {
    out.writeStartElement("datalogger");

    out << dataLogger.target << dataLogger.clockPin << dataLogger.clockFreq;
    out << dataLogger.clockDivide << dataLogger.clockMultiply;
    out << dataLogger.expertMode;

    if (dataLogger.isExpertMode()) {
        out << dataLogger.automaticCoreAssigner;
    }

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
        cerr << QObject::tr("Error: File is not a fpga-log datalogger!").toStdString() << endl;
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
            QObject::connect(dso, SIGNAL(viasChanged()), &dataLogger, SLOT(viaChanged()));
        } else if (in.name().compare("CObject") == 0) {
            dataLogger.otherObjects.push_back(new CObject(in, &dataLogger));
        } else if (in.name().compare("parameter") == 0) {
            if (in.attributes().value("name").compare("clock divide") == 0) {
                dataLogger.clockDivide.loadFromXmlStream(in);
            } else if (in.attributes().value("name").compare("clock multiply") == 0) {
                dataLogger.clockMultiply.loadFromXmlStream(in);
            } else if (in.attributes().value("name").compare("expert mode") == 0) {
                dataLogger.expertMode.loadFromXmlStream(in);
            }
        } else if (in.name().compare("AutomaticCoreAssigner") == 0) {
            in >> dataLogger.automaticCoreAssigner;
        } else
            in.skipCurrentElement();
    }

    for (map<PortOut*, stringPair>::iterator i = connections.begin(); i != connections.end(); i++) {
        PortOut* p = i->first;
        Port* dest = dataLogger.getDatastreamObject(i->second.first)->getPort(i->second.second);
        if (dest != NULL)
            p->connectPort(dest);
    }

    dataLogger.expertModeParamChanged(dataLogger.getExpertMode()->getValue());

    emit dataLogger.datastreamModulesChanged();
    emit dataLogger.otherModulesChanged();
    emit dataLogger.connectionsChanged();

    return in;
}

void DataLogger::addObject(string name, bool isDataStreamObject, QXmlStreamReader& description, QPoint pos) {
    if (isDataStreamObject) {
        DatastreamObject* dso = new DatastreamObject(description, this, name, true);
        dso->setPosition(pos);
        datastreamObjects.push_back(dso);
        connect(dso, SIGNAL(connectionsChanged()), this, SLOT(moduleConnectionsChanged()));
        connect(dso, SIGNAL(viasChanged()), this, SLOT(viaChanged()));
        emit datastreamModulesChanged();
    } else {
        CObject* co = new CObject(description, this, name, true);
        otherObjects.push_back(co);
        emit otherModulesChanged();
    }
}

void DataLogger::addCoreConnector(DatastreamObject *module, PortOut* port, bool contolStream) {
    Port* destinationPort = port->getDestination();
    DatastreamObject* destinationModule = destinationPort->getParent();

    string sinkName = module->getName() + "_" + port->getName() + "_connector";
    DataTypeStruct* sinkType;
    DataTypeStruct* sourceType;
    if (contolStream) {
        sinkType = DataTypeStruct::getType("dm_core_connector_control_sink_t");
        sourceType = DataTypeStruct::getType("dm_core_connector_control_source_t");
    } else {
        sinkType = DataTypeStruct::getType("dm_core_connector_data_sink_t");
        sourceType = DataTypeStruct::getType("dm_core_connector_data_source_t");
    }
    DatastreamObject* coreConnectorSink = new DatastreamObject(sinkName, sinkType, this);
    addDataStreamObject(coreConnectorSink);
    coreConnectorSink->setSpartanMcCore(module->getSpartanMcCore());

    DatastreamObject* coreConnectorSource = new DatastreamObject(destinationModule->getName() + "_" + destinationPort->getName() + "_connector", sourceType, this);
    addDataStreamObject(coreConnectorSource);
    coreConnectorSource->setSpartanMcCore(destinationModule->getSpartanMcCore());

    if (contolStream) {
        port->connectPort(coreConnectorSink->getPort("control_in"));
        destinationPort->connectPort(coreConnectorSource->getPort("control_out"));
    } else {
        port->connectPort(coreConnectorSink->getPort("data_in"));
        destinationPort->connectPort(coreConnectorSource->getPort("data_out"));
    }

    transform(sinkName.begin(), sinkName.end(), sinkName.begin(), ::toupper);
    string sourcePrefix = "SUBSYSTEM_" + to_string(module->getSpartanMcCore()) + "/" + sinkName + "_CORE_CONNECTOR/#PORT.";
    SpmcPeripheral* conPeriph = *coreConnectorSource->getPeripherals().begin();
    conPeriph->setFirstPortLine("Signals to connector-master", "read_enable", sourcePrefix+"read_enable");
    conPeriph->setFirstPortLine("Signals from connector-master", "data_from_master", sourcePrefix+"data_to_slave");
    conPeriph->setFirstPortLine("Signals from connector-master", "free_entries", sourcePrefix+"free_entries");
    conPeriph->setFirstPortLine("Signals from connector-master", "used_entries", sourcePrefix+"used_entries");
}

void DataLogger::addCoreConnectors() {
    for (list<DatastreamObject*>::iterator i = datastreamObjects.begin(); i != datastreamObjects.end(); i++) {
        DatastreamObject* module = *i;

        list<PortOut*> portsData = module->getOutPorts(PORT_TYPE_DATA_OUT);
        list<PortOut*> portsContol = module->getOutPorts(PORT_TYPE_CONTROL_OUT);

        for (list<PortOut*>::iterator portIt = portsContol.begin(); portIt != portsContol.end(); portIt++) {
            PortOut* port = *portIt;
            if (port->isConnected()) {
                if (module->getSpartanMcCore() != port->getDestination()->getParent()->getSpartanMcCore()) {
                    cout << tr("control core connector needed:").toStdString() << module->getName() << " -> " << port->getDestination()->getParent()->getName() << endl;
                    addCoreConnector(module, port, true);
                }
            }
        }
        for (list<PortOut*>::iterator portIt = portsData.begin(); portIt != portsData.end(); portIt++) {
            PortOut* port = *portIt;
            if (port->isConnected()) {
                if (module->getSpartanMcCore() != port->getDestination()->getParent()->getSpartanMcCore()) {
                    cout << tr("data core connector needed:").toStdString() << module->getName() << " -> " << port->getDestination()->getParent()->getName() << endl;
                    addCoreConnector(module, port, false);
                }
            }
        }
    }
}

void DataLogger::expertModeParamChanged(string value) {
    emit expertModeChanged(value.compare("TRUE") == 0);
}
