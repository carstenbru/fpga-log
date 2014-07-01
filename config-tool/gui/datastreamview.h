#ifndef DATASTREAMVIEW_H
#define DATASTREAMVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <map>
#include "datalogger.h"
#include "portbutton.h"

#define CONNECTION_LINE_SPACE 20
#define LINE_WIDTH 2

#define MODULE_SIZE_X 150

class DatastreamView : public QObject
{
    Q_OBJECT
public:
    explicit DatastreamView(QGraphicsView* view, DataLogger* dataLogger);

private:
    void generateModuleGui(DatastreamObject* datastreamObject);
    void deleteAllModuleGuis();

    QGraphicsView* view;
    DataLogger* dataLogger;

    std::map<Port*, PortButton*> ports;
    std::list<PortOutButton*> portOuts;

    std::map<QWidget*, DatastreamObject*> moduleGuiElements;
public slots:
    void setModulePositions();
    void redrawModules();
    void redrawStreams();
    void moveDatastreamModules();
private slots:
    void configClickedModule();
signals:
    void requestConfigDialog(CObject& object);
};

#endif // DATASTREAMVIEW_H
