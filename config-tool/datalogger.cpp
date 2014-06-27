#include "datalogger.h"
#include <sstream>
#include <iostream>

using namespace std;

DataLogger::DataLogger() :
    peripheralClockFreq("peripheralClockFreq", DataType::getType("peripheral_int"), false, "1000") //TODO value
{
    peripheralClockFreq.setHideFromUser(true);
}

DataLogger::~DataLogger() {
    std::list<DatastreamObject*>::iterator i;
    for (i = datastreamObjects.begin(); i != datastreamObjects.end(); i++) {
        delete *i;
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
