#include "datalogger.h"
#include <sstream>

using namespace std;

DataLogger::DataLogger()
{
}

DataLogger::~DataLogger() {
    std::list<DatastreamObject*>::iterator i;
    for (i = datastreamObjects.begin(); i != datastreamObjects.end(); i++) {
        delete *i;
    }
}

void DataLogger::newObject(DataType *type) {
    if (type->hasPrefix("device_") || type->hasPrefix("dm_") || type->hasPrefix("sink_")) {
        DatastreamObject* dso = new DatastreamObject(findObjectName(datastreamObjects, type), type);
        datastreamObjects.push_back(dso);
        connect(dso, SIGNAL(connectionsChanged()), this, SLOT(moduleConnectionsChanged()));
        emit datastreamModulesChanged();
    }
    else {
        CObject* co = new CObject(findObjectName(otherObjects, type), type); //TODO name!
        otherObjects.push_back(co);
        emit otherModulesChanged();
    }
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
