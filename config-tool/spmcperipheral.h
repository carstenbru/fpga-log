#ifndef SPMCPERIPHERAL_H
#define SPMCPERIPHERAL_H

#include <QObject>
#include <list>
#include <string>
#include <map>
#include <QXmlStreamReader>
#include "datatype.h"
#include "cparameter.h"

class PeripheralPort : public QObject {
    Q_OBJECT

public:
    PeripheralPort(std::string name);
    PeripheralPort(std::string name, int width);
    ~PeripheralPort();

    std::list<CParameter*> getLines() { return lines; }
private:
    std::string name;
    int width;

    std::list<CParameter*> lines;
public slots:
    void newWidth(std::string widthVal);
};

class SpmcPeripheral
{
public:
    SpmcPeripheral(DataType* dataType, std::string parentModuleName, DataLogger *dataLogger);
    ~SpmcPeripheral();

    std::list<CParameter*> getParameters() { return parameters; }
    CParameter* getParameter(std::string name);
    std::map<std::string, std::list<PeripheralPort*> > getPorts() { return ports; }

    static void loadPeripheralXMLs();
private:
    std::string getFileName();
    void readParameterElement(QXmlStreamReader& reader);
    void readPortsElement(QXmlStreamReader& reader);
    void readPeripheralXML();
    void readModuleXML();

    static std::string readModuleNameFromFile(std::string fileName);

    DataLogger* dataLogger;

    DataType* dataType;
    std::list<CParameter*> parameters;
    std::map<std::string, std::list<PeripheralPort*> > ports;

    std::string parentModuleName;

    static std::map<std::string, std::string> peripheralXMLs;
};

#endif // SPMCPERIPHERAL_H
