#ifndef DATASTREAMVIEW_H
#define DATASTREAMVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <map>
#include "datalogger.h"
#include "portbutton.h"

#define CONNECTION_LINE_SPACE 20
#define LINE_WIDTH 2
#define VIA_WIDTH 12
#define VIA_ADD_TOL 10

#define MODULE_SIZE_X 150

class DatastreamView : public QObject
{
    Q_OBJECT
public:
    explicit DatastreamView(QGraphicsView* view);

    void setDataLogger(DataLogger* dataLogger);

    void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
private:
    void generateModuleGui(DatastreamObject* datastreamObject);
    void deleteAllModuleGuis();
    void deleteAllViaBtns();

    QGraphicsView* view;
    DataLogger* dataLogger;

    std::map<Port*, PortButton*> ports;
    std::list<PortOutButton*> portOuts;

    std::map<QWidget*, DatastreamObject*> moduleGuiElements;
    std::map<QWidget*, PortOut*> viaBtns;
public slots:
    void setModulePositions();
    void redrawModules();
    bool redrawStreams(bool addVia = false, QPoint newVia = QPoint(0,0), bool keepViaBtns = false);
    void moveDatastreamModules();
private slots:
    void configClickedModule();
    void viaMoved(QPoint oldPos, QPoint newPos);
    void viaDelete();
signals:
    void requestConfigDialog(CObject& object);
};

class DatastreamViewScene : public QGraphicsScene {
public:
    DatastreamViewScene(QRectF size, DatastreamView* view) : QGraphicsScene(size), view(view) {}

protected:
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);

private:
    DatastreamView* view;
};

#endif // DATASTREAMVIEW_H
