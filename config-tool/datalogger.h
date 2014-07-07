#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <QObject>
#include <list>
#include <vector>
#include <QXmlStreamWriter>
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
    void deleteObject(CObject* object);
    std::list<DatastreamObject*> getDatastreamModules() { return datastreamObjects; }
    std::vector<CObject*> getOtherObjects() { return otherObjects; }
    std::list<CObject*> getInstances(DataTypeStruct *dataType);
    std::map<std::string, CObject*> getObjectsMap();
    bool changeObjectName(CObject* object, std::string newName);
    int getClk();
    int getPeriClk();

    void loadTargetPins();

    CParameter* getTarget() { return &target; }
    CParameter* getClockPin() { return &clockPin; }
    CParameter* getClockFreq() { return &clockFreq; }

    static void loadTragetXMLs();
    static std::string getTargetXML(std::string target) { return targetXMLs.at(target); }

    friend QXmlStreamWriter& operator<<(QXmlStreamWriter& out, DataLogger& dataLogger);
    friend QXmlStreamReader& operator>>(QXmlStreamReader& in, DataLogger& dataLogger);
private:
    template <typename T>
    bool containsObjectName(T searchList, std::string name);
    template <typename T>
    std::string findObjectName(T searchList, DataType* dataType);
    template <typename T>
    void addInstancesToList(T searchList, std::list<CObject*>& destList,  DataTypeStruct* dataType);
    template <typename T>
    bool deleteObject(T& searchList, CObject* object);

    DatastreamObject* getDatastreamObject(std::string name);

    static std::string readTargetNameFromFile(std::string fileName);

    CParameter target;
    CParameter clockPin;
    CParameter clockFreq;

    std::list<DatastreamObject*> datastreamObjects;
    std::vector<CObject*> otherObjects;

    static std::map<std::string, std::string> targetXMLs;
private slots:
    void moduleConnectionsChanged();
    void parameterChanged();
signals:
    void datastreamModulesChanged();
    void otherModulesChanged();
    void connectionsChanged();
    void criticalParameterChanged();
};

#endif // DATALOGGER_H
