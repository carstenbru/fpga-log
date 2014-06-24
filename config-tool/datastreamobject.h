#ifndef DATASTREAMOBJECT_H
#define DATASTREAMOBJECT_H

#include <QObject>
#include <string>
#include <QPoint>
#include <list>
#include "port.h"
#include "datatype.h"

class DatastreamObject : public QObject
{
    Q_OBJECT

public:
    DatastreamObject(DataType* type);
    ~DatastreamObject();

    std::list<ControlPortIn*> getControlInPorts() { return controlInPorts; }
    std::list<ControlPortOut*> getControlOutPorts() { return controlOutPorts; }
    std::list<DataPortIn*> getDataInPorts() { return dataInPorts; }
    std::list<DataPortOut*> getDataOutPorts() { return dataOutPorts; }
    std::list<PortOut*> getOutPorts(port_type type);
    std::string getDisplayName();

    void setPosition(QPoint pos);
    QPoint getPosition();
private:
    void findPorts();

    void addPort(ControlPortIn* port);
    void addPort(ControlPortOut* port);
    void addPort(DataPortIn* port);
    void addPort(DataPortOut* port);

    DataType* type;

    std::list<ControlPortIn*> controlInPorts;
    std::list<ControlPortOut*> controlOutPorts;
    std::list<DataPortIn*> dataInPorts;
    std::list<DataPortOut*> dataOutPorts;

    QPoint position;
private slots:
    void portConnectionChanged();
signals:
    void connectionsChanged();
};

#endif // DATASTREAMOBJECT_H
