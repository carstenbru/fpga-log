#ifndef COBJECT_H
#define COBJECT_H

#include <QObject>
#include <string>
#include <list>
#include "datatype.h"
#include "spmcperipheral.h"

class CObject : public QObject
{
    Q_OBJECT

public:
    CObject(std::string name, DataTypeStruct *dataType, DataLogger* dataLogger);
    CObject(QXmlStreamReader& in, DataLogger* dataLogger, std::string name = "", bool ignorePins = false);
    CObject(QXmlStreamReader& in, DataLogger* dataLogger, bool, std::string name = "", bool ignorePins = false);
    virtual ~CObject();

    void addAdvancedConfig(std::string methodName);

    std::string getName() { return name; }
    void setName(std::string name) { this->name = name; }
    DataTypeStruct* getType() { return type; }

    CMethod* getInitMethod() { return initMethod; }
    std::list<CMethod*> getAdvancedConfig() { return advancedConfig; }
    std::list<SpmcPeripheral*> getPeripherals() { return peripherals; }
    std::map<std::string, CParameter*> getTimestampPins() { return timestampPins; }
    std::list<CParameter*> getTimestampPinParameters();
    bool getTimestampPinInvert(std::string pin) { return timestampPinsInvert.at(pin); }

    void setDefinitionsUpdated(bool state) {definitionsUpdated = state;}
    bool getDefinitionsUpdated() {return definitionsUpdated;}

    friend QXmlStreamWriter& operator<<(QXmlStreamWriter& out, CObject& cObject);
protected:
    void readTimestampPinsFromModuleXml();

    bool setPeripheral(SpmcPeripheral* newPeripheral);

    std::string name;
    DataTypeStruct* type;

    CMethod* initMethod;
    std::list<SpmcPeripheral*> peripherals;
    std::map<std::string, CParameter*> timestampPins;
    std::map<std::string, bool> timestampPinsInvert;

    std::list<CMethod*> advancedConfig;

    bool definitionsUpdated;
public slots:
    void removeAdvancedConfig(int methodId);
signals:
    void advancedConfigRemoved();
};

#endif // COBJECT_H
