#ifndef DATASTREAMOBJECT_H
#define DATASTREAMOBJECT_H

#include <string>
#include <QPoint>
#include <list>
#include "cobject.h"
#include "port.h"
#include "datatype.h"

class DatastreamObject : public CObject
{
    Q_OBJECT

public:
    DatastreamObject(std::string name, DataType* type);
    ~DatastreamObject();

    std::list<ControlPortIn*> getControlInPorts() { return controlInPorts; }
    std::list<ControlPortOut*> getControlOutPorts() { return controlOutPorts; }
    std::list<DataPortIn*> getDataInPorts() { return dataInPorts; }
    std::list<DataPortOut*> getDataOutPorts() { return dataOutPorts; }
    std::list<PortOut*> getOutPorts(port_type type);

    void setPosition(QPoint pos);
    QPoint getPosition();
private:
    void findPorts();
    template <typename T>
    bool reorderUnconnectedPort(std::list<T*>& ports, Port* port);

    void addPort(ControlPortIn* port);
    void addPort(ControlPortOut* port);
    void addPort(DataPortIn* port);
    void addPort(DataPortOut* port);

    std::list<ControlPortIn*> controlInPorts;
    std::list<ControlPortOut*> controlOutPorts;
    std::list<DataPortIn*> dataInPorts;
    std::list<DataPortOut*> dataOutPorts;

    QPoint position;
private slots:
    void portConnected();
    void portDisconnected(Port *port);
signals:
    void connectionsChanged();
};

#endif // DATASTREAMOBJECT_H
