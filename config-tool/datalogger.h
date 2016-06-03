#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <QObject>
#include <list>
#include <vector>
#include <QXmlStreamWriter>
#include "datatype.h"
#include "cobject.h"
#include "datastreamobject.h"
#include "automaticcoreassigner.h"

class DataLogger : public QObject
{
    Q_OBJECT

public:
    DataLogger();
    DataLogger(DataLogger& other);
    ~DataLogger();

    std::string newObject(DataTypeStruct *type);
    void addDataStreamObject(DatastreamObject *dso);
    void addObject(std::string name, bool isDataStreamObject, QXmlStreamReader& description, QPoint pos = QPoint(0,0));
    void deleteObject(CObject* object);
    std::list<DatastreamObject*> getDatastreamModules() { return datastreamObjects; }
    std::vector<CObject*> getOtherObjects() { return otherObjects; }
    std::list<CObject*> getInstances(DataTypeStruct *dataType);
    std::map<std::string, CObject*> getObjectsMap();
    bool changeObjectName(CObject* object, std::string newName);
    int getClk();
    int getSystemClk();
    int getPeriClk();
    int getClkDivide();
    int getClkMultiply();

    void loadTargetPins();

    void addCoreConnectors();
    void addCoreConnector(DatastreamObject* module,PortOut* port, bool contolStream);

    CParameter* getTarget() { return &target; }
    CParameter* getClockPin() { return &clockPin; }
    CParameter* getClockFreq() { return &clockFreq; }
    CParameter* getClockDivideParam() { return &clockDivide; }
    CParameter* getClockMultiplyParam() { return &clockMultiply; }
    CParameter* getExpertMode() { return &expertMode; }

    bool isExpertMode() { return expertMode.getValue().compare("TRUE") == 0; }

    void setDefinitionsUpdated(bool state) {definitionsUpdated = state;}
    bool getDefinitionsUpdated() {return definitionsUpdated;}
    void addDefinitionsUpdatedModule(std::string moduleName) {definitionsUpdatedModules += "\n" + moduleName; definitionsUpdated = true;}
    std::string getDefinitionsUpdatedModules() { return definitionsUpdatedModules; }
    std::map<CParameter *, std::string[4]> getPinAssignments();

    std::string findObjectName(bool isDataStreamObject, DataType* dataType);

    static void loadTragetXMLs();
    static std::string getTargetXML(std::string target) { return targetXMLs.at(target); }

    AutomaticCoreAssigner* getAutomaticCoreAssigner() { return &automaticCoreAssigner; }

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

    CParameter clockDivide;
    CParameter clockMultiply;

    CParameter expertMode;

    std::list<DatastreamObject*> datastreamObjects;
    std::vector<CObject*> otherObjects;

    static std::map<std::string, std::string> targetXMLs;

    bool definitionsUpdated;
    std::string definitionsUpdatedModules;

    AutomaticCoreAssigner automaticCoreAssigner;
private slots:
    void moduleConnectionsChanged();
    void viaChanged();
    void expertModeParamChanged(std::string value);
public slots:
    void parameterChanged();
    void pinParamterChanged();
signals:
    void datastreamModulesChanged();
    void otherModulesChanged();
    void connectionsChanged();
    void viasChanged();
    void criticalParameterChanged();
    void pinChanged();
    void storePins();
    void expertModeChanged(bool expertMode);
};

#endif // DATALOGGER_H
