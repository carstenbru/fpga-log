#include "datalogger.h"

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
