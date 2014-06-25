#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <QObject>
#include <list>
#include <vector>
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

#endif // DATALOGGER_H
