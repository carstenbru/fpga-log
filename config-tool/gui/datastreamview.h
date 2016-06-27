/**
 * @file datastreamview.h
 * @brief DatastreamView and DatastreamViewScene GUI classes
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef DATASTREAMVIEW_H
#define DATASTREAMVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <map>
#include "datalogger.h"
#include "portbutton.h"


//space between a connection line and the port it is connected to
#define CONNECTION_LINE_SPACE 20
//port connection line widths
#define LINE_WIDTH 2
//graphical VIA size
#define VIA_WIDTH 12
//tolerance between cursor position and connection wire which is still considered to be on the wire
#define VIA_ADD_TOL 10
//size of a module in the DatastreamView
#define MODULE_SIZE_X 150

/**
 * @brief datastream view class for drawing modules, connections and UI in the graphical logger view
 */
class DatastreamView : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
     * @param view destination view on which this class draws
     */
    explicit DatastreamView(QGraphicsView* view);

    /**
     * @brief sets the datalogger this view belongs to
     *
     * @param dataLogger the datalogger this view belongs to
     */
    void setDataLogger(DataLogger* dataLogger);
    /**
     * @brief sets if pasting a module is possible (i.e. a object description is in the clipboard)
     *
     * @param pastePossible true if pasting is possible, otherwise false
     */
    void setPastePossible(bool pastePossible) { this->pastePossible = pastePossible; }

    /**
     * @brief mouse release event, called when a mouse button was released on the scene by the DatastreamViewScene class
     *
     * @param mouseEvent release event
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
private:
    void generateModuleGui(DatastreamObject* datastreamObject);
    void deleteAllModuleGuis();
    void raiseModules();
    void deleteAllViaBtns();

    bool addCustomVIAs(std::list<std::pair<QPoint, bool>>& viaList, PortOut *p, bool keepViaBtns, QColor color);
    void refineVIAs(std::list<std::pair<QPoint, bool>>& viaList);

    QGraphicsView* view;
    DataLogger* dataLogger;

    std::map<Port*, PortButton*> ports;
    std::list<PortOutButton*> portOuts;

    std::map<QWidget*, DatastreamObject*> moduleGuiElements;
    std::map<QWidget*, PortOut*> viaBtns;

    bool pastePossible;
    QPoint lastMenuPos;
    DatastreamObject* rightClickedObject;
public slots:
    /**
     * @brief slot to set the positions of all modules again
     */
    void setModulePositions();
    /**
     * @brief slot to redraw all modules in the view
     */
    void redrawModules();
    /**
     * @brief redraws the streams in the DatastreamView
     *
     * This function also handles adding of VIAs
     *
     * @param addVia if true the Via specified in newVia will be added (if on a connection)
     * @param newVia position at which a Via should be added
     * @param keepViaBtns if true keep the old Buttons for Vias (necessary if Vias moved), if false generate new ones (necessary if Vias changed)
     *
     * @return true if a VIA was successfully added
     */
    bool redrawStreams(bool addVia = false, QPoint newVia = QPoint(0,0), bool keepViaBtns = false);
    /**
     * @brief moves all datastream modules in the view to the correct position, neccessary when a scrollbar was used
     */
    void moveDatastreamModules();
private slots:
    void configClickedModule();
    void moduleRightClick(QPoint pos);
    void viaMoved(QPoint oldPos, QPoint newPos);
    void viaDelete();

    void pasteModule();
    void copyModule();
signals:
    /**
     * @brief emitted when a configuration dialog for a DatastreamObject is requested
     * @param object
     */
    void requestConfigDialog(CObject& object);
    /**
     * @brief emitted when a new module should be pasted
     *
     * @param pos position of the new module
     */
    void pasteModule(QPoint pos);
    /**
     * @brief emitted when a object should be copied (to the clipboard)
     *
     * @param objectDescription XML representation of the object
     */
    void copyObject(std::string objectDescription);
};

/**
 * @brief scene class (which holds drawings of the connections) used by the DatastreamView
 */
class DatastreamViewScene : public QGraphicsScene {
public:
    /**
     * @brief constructor
     *
     * @param size scene size
     * @param view parent DatastreamView
     */
    DatastreamViewScene(QRectF size, DatastreamView* view) : QGraphicsScene(size), view(view) {}

protected:
    /**
     * @brief inherited mouse release event, called when a mouse button was released on the scene
     *
     * @param mouseEvent release event
     */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);

private:
    DatastreamView* view;
};

#endif // DATASTREAMVIEW_H
