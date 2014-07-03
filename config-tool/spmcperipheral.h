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

class PeripheralPort : public QObject {
    Q_OBJECT

public:
    PeripheralPort(std::string name);
    PeripheralPort(std::string name, int width);
    ~PeripheralPort();

    void setDirection(std::string direction);
    std::list<CParameter*> getLines() { return lines; }
    std::string getName() { return name; }
    std::string getDirection() { return direction; }
    void setHideFromUser(bool hide);
private:
    std::string name;
    std::string direction;
    int width;

    std::list<CParameter*> lines;
public slots:
    void newWidth(std::string widthVal);
};

class SpmcPeripheral
{
public:
    SpmcPeripheral(std::string name, DataType* dataType, CObject* parentObject, DataLogger *dataLogger);
    ~SpmcPeripheral();

    std::list<CParameter*> getParameters() { return parameters; }
    CParameter* getParameter(std::string name);
    PeripheralPort* getPort(std::string group, std::string name);
    std::map<std::string, std::list<PeripheralPort*> > getPorts() { return ports; }
    std::string getCompleteName();
    DataType* getDataType() { return dataType; }

    static void loadPeripheralXMLs();
private:
    std::string getFileName();
    void readParameterElement(QXmlStreamReader& reader);
    void readPortsElement(QXmlStreamReader& reader, std::string direction);
    void readPeripheralXML();
    void readModuleXML();

    static std::string readModuleNameFromFile(std::string fileName);

    std::string name;
    DataLogger* dataLogger;

    DataType* dataType;
    std::list<CParameter*> parameters;
    std::map<std::string, std::list<PeripheralPort*> > ports;

    CObject* parentObject;

    static std::map<std::string, std::string> peripheralXMLs;
};

#endif // SPMCPERIPHERAL_H
