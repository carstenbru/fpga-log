#ifndef SPMCPERIPHERAL_H
#define SPMCPERIPHERAL_H

#include <QObject>
#include <list>
#include <string>
#include <map>
#include <QXmlStreamReader>
#include "datatype.h"
#include "cparameter.h"

class CObject;
class SpmcPeripheral;

class PeripheralPort : public QObject {
    Q_OBJECT

public:
    PeripheralPort(std::string name);
    PeripheralPort(std::string name, CParameter* widthRef);
    PeripheralPort(std::string name, int width);
    ~PeripheralPort();

    void load(QXmlStreamReader& in, CObject *parent, SpmcPeripheral *peripheral);
    bool setLine(CParameter* newValue);

    void setDirection(std::string direction);
    std::list<CParameter*> getLines() { return lines; }
    std::string getName() { return name; }
    std::string getDirection() { return direction; }
    void setHideFromUser(bool hide);
    void setConstraints(std::string constraints) { this->constraints = constraints; }
    std::string getConstraints() { return constraints; }
    std::string getWidthRef() { return widthRef; }

    friend QXmlStreamWriter& operator<<(QXmlStreamWriter& out, PeripheralPort& port);
private:
    std::string name;
    std::string direction;

    std::string widthRef;

    std::list<CParameter*> lines;

    std::string constraints;
public slots:
    void newWidth(std::string widthVal);
};

class SpmcPeripheral
{
public:
    SpmcPeripheral(std::string name, DataType* dataType, CObject* parentObject, DataLogger *dataLogger);
    SpmcPeripheral(QXmlStreamReader& in, CObject* parentObject, DataLogger *dataLogger, bool ignorePins = false);
    ~SpmcPeripheral();

    std::list<CParameter*> getParameters() { return parameters; }
    CParameter* getParameter(std::string name);
    bool setParameter(CParameter* newValue);
    PeripheralPort* getPort(std::string group, std::string name);
    std::map<std::string, std::list<PeripheralPort*> > getPorts() { return ports; }
    std::string getCompleteName();
    std::string getParentName();
    CObject* getParentObject() { return parentObject; }
    DataType* getDataType() { return dataType; }
    bool hasDMA() {return hasDMAmemory; }

    static void loadPeripheralXMLs();

    friend QXmlStreamWriter& operator<<(QXmlStreamWriter& out, SpmcPeripheral& peripheral);
private:
    std::string getFileName();
    void readParameterElement(QXmlStreamReader& reader);
    void readPortsElement(QXmlStreamReader& reader, std::string direction);
    void readPeripheralXML();
    void readModuleXML();

    static std::string readModuleNameFromFile(std::string fileName);

    void saveToXml(QXmlStreamWriter& out);

    std::string name;
    DataLogger* dataLogger;
    bool hasDMAmemory;

    DataType* dataType;
    std::list<CParameter*> parameters;
    std::map<std::string, std::list<PeripheralPort*> > ports;

    CObject* parentObject;

    static std::map<std::string, std::string> peripheralXMLs;
};

#endif // SPMCPERIPHERAL_H
