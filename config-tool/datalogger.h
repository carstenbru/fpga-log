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

    void newObject(DataTypeStruct *type);
    std::list<DatastreamObject*> getDatastreamModules() { return datastreamObjects; }
    std::vector<CObject*> getOtherObjects() { return otherObjects; }
    std::list<CObject*> getInstances(DataTypeStruct *dataType);
    bool changeObjectName(CObject* object, std::string newName);

    void loadTargetPins();

    CParameter* getTarget() { return &target; }
    CParameter* getClockPin() { return &clockPin; }

    static void loadTragetXMLs();
private:
    template <typename T>
    bool containsObjectName(T searchList, std::string name);
    template <typename T>
    std::string findObjectName(T searchList, DataType* dataType);
    template <typename T>
    void addInstancesToList(T searchList, std::list<CObject*>& destList,  DataTypeStruct* dataType);

    static std::string readTargetNameFromFile(std::string fileName);

    CParameter target;
    CParameter clockPin;

    std::list<DatastreamObject*> datastreamObjects;
    std::vector<CObject*> otherObjects;

    static std::map<std::string, std::string> targetXMLs;
private slots:
    void moduleConnectionsChanged();
signals:
    void datastreamModulesChanged();
    void otherModulesChanged();
    void connectionsChanged();
};

#endif // DATALOGGER_H
