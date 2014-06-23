#include "datalogger.h"

DataLogger::DataLogger()
{
}

DataLogger::~DataLogger() {
    std::list<DatastreamObject*>::iterator i;
    for (i = datastreamObjects.begin(); i != datastreamObjects.end(); i++) {
        delete *i;
    }
}

std::list<DatastreamObject*> DataLogger::getModules() {
    return datastreamObjects;
}

void DataLogger::newDatastreamObject(std::string type) {
    DatastreamObject* dso = new DatastreamObject(type);
    datastreamObjects.push_back(dso);
    connect(dso, SIGNAL(connectionsChanged()), this, SLOT(moduleConnectionsChanged()));
    emit modulesChanged();
}

void DataLogger::moduleConnectionsChanged() {
    emit connectionsChanged();
}
