#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <QObject>
#include <list>
#include "datastreamobject.h"

class DataLogger : public QObject
{
    Q_OBJECT

public:
    DataLogger();
    ~DataLogger();

    void newDatastreamObject(std::string type);
    std::list<DatastreamObject*> getModules();
private:
    std::list<DatastreamObject*> datastreamObjects;
private slots:
    void moduleConnectionsChanged();
signals:
    void modulesChanged();
    void connectionsChanged();
};

#endif // DATALOGGER_H
