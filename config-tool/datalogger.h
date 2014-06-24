#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <QObject>
#include <list>
#include <vector>
#include <sstream>
#include "datatype.h"
#include "cobject.h"
#include "datastreamobject.h"

class DataLogger : public QObject
{
    Q_OBJECT

public:
    DataLogger();
    ~DataLogger();

    void newObject(DataType* type);
    std::list<DatastreamObject*> getDatastreamModules() {return datastreamObjects; }
    std::vector<CObject*> getOtherObjects() { return otherObjects; }
private:
    template <typename T>
    bool containsObjectName(T searchList, std::string name);
    template <typename T>
    std::string findObjectName(T searchList, DataType* dataType);

    std::list<DatastreamObject*> datastreamObjects;
    std::vector<CObject*> otherObjects;
private slots:
    void moduleConnectionsChanged();
signals:
    void datastreamModulesChanged();
    void otherModulesChanged();
    void connectionsChanged();
};

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
    std::stringstream ss;
    do {
        ss.str(std::string());
        ss << dataType->getDisplayName();
        ss << "_";
        ss << i;
        i++;
    } while (containsObjectName(searchList, ss.str()));

    return ss.str();
}

#endif // DATALOGGER_H
