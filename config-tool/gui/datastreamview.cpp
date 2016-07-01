/**
 * @file datastreamview.cpp
 * @brief DatastreamView and DatastreamViewScene GUI classes
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include "datastreamview.h"
#include "moveablebutton.h"
#include <QScrollBar>
#include <QWidget>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QAction>

using namespace std;

void DatastreamViewScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent) {
    view->mouseReleaseEvent(mouseEvent);
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void DatastreamView::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent) {
    if (mouseEvent->button() == Qt::LeftButton) {
        if (redrawStreams(true, mouseEvent->scenePos().toPoint())) {
            redrawStreams();
        }
    } else {
        if (mouseEvent->button() == Qt::RightButton) {
            if (pastePossible) {
                int x = view->horizontalScrollBar()->value();
                int y = view->verticalScrollBar()->value();
                QPoint p(x, y);
                lastMenuPos = mouseEvent->scenePos().toPoint();

                QMenu *menu = new QMenu(view);
                QAction* action = new QAction(tr("Paste object"), this);
                connect(action, SIGNAL(triggered(bool)), this, SLOT(pasteModule()));
                menu->addAction(action);
                menu->popup(view->mapToGlobal(mouseEvent->scenePos().toPoint() - p));
            }
        }
    }
}

void DatastreamView::pasteModule() {
    emit pasteModule(lastMenuPos);
}

void DatastreamView::copyModule() {
    QString s;
    QXmlStreamWriter stream(&s);
    stream << *((CObject*)rightClickedObject);
    emit copyObject(s.toStdString());
}

DatastreamView::DatastreamView(QGraphicsView* view) :
    view(view),
    dataLogger(NULL),
    pastePossible(false)
{
    view->setScene(new DatastreamViewScene(QRectF(0, 0, 5300, 3005), this)); //TODO scene size

    connect(view->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(moveDatastreamModules()));
    connect(view->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(moveDatastreamModules()));

    redrawModules();
}

void DatastreamView::setDataLogger(DataLogger* dataLogger) {
    this->dataLogger = dataLogger;
    connect(dataLogger, SIGNAL(datastreamModulesChanged()), this, SLOT(redrawModules()));
    connect(dataLogger, SIGNAL(connectionsChanged()), this, SLOT(redrawModules()));
}

void DatastreamObject::setPosition(QPoint pos) {
    position = pos;
}

QPoint DatastreamObject::getPosition() {
    return position;
}

void DatastreamView::deleteAllModuleGuis() {
    for (std::map<Port*, PortButton*>::iterator p = ports.begin(); p != ports.end(); p++) {
        p->second->deleteLater();
    }
    ports.clear();
    portOuts.clear();

    std::map<QWidget*, DatastreamObject*>::iterator i;
    for (i = moduleGuiElements.begin(); i != moduleGuiElements.end(); i++) {
        i->first->deleteLater();
    }
    moduleGuiElements.clear();
}

void DatastreamView::generateModuleGui(DatastreamObject* datastreamObject) {
    MoveableButton* btn = new MoveableButton();
    btn->setParent(view->viewport());
    btn->setText(QString::fromStdString(datastreamObject->getName()));
    btn->setStyleSheet("QWidget {background-color: #00689d ;\nfont: 10pt \"Arial\";\ncolor:white}");
    btn->setToolTip(datastreamObject->getType()->getDescription().c_str());
    btn->setFixedSize(MODULE_SIZE_X, 10);
    moduleGuiElements[btn] = datastreamObject;

    connect(btn, SIGNAL(moved()), this, SLOT(setModulePositions()));
    connect(btn, SIGNAL(moved()), this, SLOT(redrawStreams()));
    connect(btn, SIGNAL(clicked()), this, SLOT(configClickedModule()));
    connect(btn, SIGNAL(rightClicked(QPoint)), this, SLOT(moduleRightClick(QPoint)));

    int port_number_left = 0;
    string lastName;
    bool lastConnected = true;
    std::list<DataPortOut*> l1 = datastreamObject->getDataOutPorts();
    for (std::list<DataPortOut*>::iterator it = l1.begin(); it != l1.end(); it++) {
        string portName = (*it)->getName();

        if (lastConnected || (portName.compare(lastName) != 0)) {
            DataPortOutButton* c = new DataPortOutButton(btn, *it, datastreamObject, port_number_left++);
            ports[*it] = (c);
            portOuts.push_back(c);
            lastName = portName;
        }

        lastConnected = (*it)->isConnected();
    }
    std::list<ControlPortIn*> l2 = datastreamObject->getControlInPorts();
    for (std::list<ControlPortIn*>::iterator it = l2.begin(); it != l2.end(); it++) {
       ports[*it] = (new ControlPortInButton(btn, *it, datastreamObject, port_number_left++));
    }

    int port_number_right = 0;
    std::list<DataPortIn*> l3 = datastreamObject->getDataInPorts();
    for (std::list<DataPortIn*>::iterator it = l3.begin(); it != l3.end(); it++) {
        ports[*it] = (new DataPortInButton(btn, *it, datastreamObject, port_number_right++));
    }
    std::list<ControlPortOut*> l4 = datastreamObject->getControlOutPorts();
    for (std::list<ControlPortOut*>::iterator it = l4.begin(); it != l4.end(); it++) {
        string portName = (*it)->getName();

        if (lastConnected || (portName.compare(lastName) != 0)) {
            ControlPortOutButton* c = new ControlPortOutButton(btn, *it, datastreamObject, port_number_right++);
            ports[*it] = (c);
            portOuts.push_back(c);
            lastName = portName;
        }

        lastConnected = (*it)->isConnected();
    }

    int maxPorts = std::max(port_number_left, port_number_right);
    maxPorts = std::max(maxPorts, 1);
    int ySize = 2 * PORT_BUTTON_Y_TOP + maxPorts * (PORT_BUTTON_Y_SPACE + PORT_BUTTON_SIZE_Y) - PORT_BUTTON_Y_SPACE;
    btn->setFixedSize(MODULE_SIZE_X, ySize);

    btn->show();
}

void DatastreamView::setModulePositions() {
    int x = view->horizontalScrollBar()->value();
    int y = view->verticalScrollBar()->value();
    QPoint p(x, y);

    std::map<QWidget*, DatastreamObject*>::iterator i;
    for (i = moduleGuiElements.begin(); i != moduleGuiElements.end(); i++) {
        i->second->setPosition(i->first->pos() + p);
    }
}

void DatastreamView::redrawModules() {
    deleteAllModuleGuis();

    if (dataLogger != NULL) {
        std::list<DatastreamObject*> modules = dataLogger->getDatastreamModules();

        std::list<DatastreamObject*>::iterator i;
        for (i = modules.begin(); i != modules.end(); i++) {
            generateModuleGui(*i);
        }

        moveDatastreamModules();

        redrawStreams();
    }
}

void DatastreamView::deleteAllViaBtns() {
    std::map<QWidget*, PortOut*>::iterator i;
    for (i = viaBtns.begin(); i != viaBtns.end(); i++) {
        i->first->deleteLater();
    }
    viaBtns.clear();
}

/**
 * @brief checks if a point is on or close to a line
 *
 * @param line to check
 * @param point point to check
 */
bool checkOnLine(QLine& line, QPoint& point) {
    int a, b;
    int c;
    if (line.dx() == 0) {
        a = line.p1().y();
        b = line.p2().y();
        c = point.y();
        if (point.x() < line.p1().x() - VIA_ADD_TOL) {
            return false;
        }
        if (point.x() > line.p1().x() + VIA_ADD_TOL) {
            return false;
        }
    } else {
        a = line.p1().x();
        b = line.p2().x();
        c = point.x();
        if (point.y() < line.p1().y() - VIA_ADD_TOL) {
            return false;
        }
        if (point.y() > line.p1().y() + VIA_ADD_TOL) {
            return false;
        }
    }
    if (a > b) {
        swap(a ,b);
    }
    if (c < a || c > b) {
        return false;
    }
    return true;
}

bool DatastreamView::addCustomVIAs(list<pair<QPoint, bool>>& viaList, PortOut* p, bool keepViaBtns, QColor color) {
    list<QPoint> customVias = p->getVias();
    int x = view->horizontalScrollBar()->value();
    int y = view->verticalScrollBar()->value();
    QPoint scrollPos(x, y);
    for (list<QPoint>::iterator i = customVias.begin(); i != customVias.end(); i++) {
        if (!keepViaBtns) {
            //create VIA button
            MoveableButton* btn = new MoveableButton();
            btn->setParent(view->viewport());
            btn->setStyleSheet("QWidget {background-color: " + color.name() + " ;\ncolor:white;\nborder-radius: " + QString::number(VIA_WIDTH/2) + "px;}");
            btn->setFixedSize(VIA_WIDTH, VIA_WIDTH);
            viaBtns[btn] = p;
            btn->move(QPoint(i->x()-VIA_WIDTH/2, i->y()-VIA_WIDTH/2) - scrollPos);
            btn->show();

            connect(btn, SIGNAL(moved(QPoint, QPoint)), this, SLOT(viaMoved(QPoint, QPoint)));
            connect(btn, SIGNAL(rightClicked(QPoint)), this, SLOT(viaDelete()));
        }

        //add it to the list
        viaList.push_back(pair<QPoint, bool>(*i, true));
    }
    //return if we added some VIAs here
    return !customVias.empty();
}

void DatastreamView::refineVIAs(list<pair<QPoint, bool>>& viaList) {
    list<pair<QPoint, bool>>::iterator lastVia = viaList.begin();
    int lastDir = -1;
    for (list<pair<QPoint, bool>>::iterator via = ++viaList.begin(); via != viaList.end(); via++) {
        int xDiff = via->first.x() - lastVia->first.x();
        int yDiff = via->first.y() - lastVia->first.y();

        if ((xDiff != 0) && (yDiff != 0)) {
            int xDirInv = xDiff < 0 ? 1 : 0;
            int yDirInv = yDiff < 0 ? 3 : 2;
            if (((lastDir >= 2) && (yDirInv != lastDir)) || (xDirInv == lastDir)) {
                viaList.insert(via, pair<QPoint, bool>(QPoint(lastVia->first.x(), via->first.y()), false));
                yDiff = 0;
            } else {
                viaList.insert(via, pair<QPoint, bool>(QPoint(via->first.x(), lastVia->first.y()), false));
                xDiff = 0;
            }
        }
        if (xDiff != 0) {
            lastDir = xDiff < 0 ? 0 : 1;
        } else {
            lastDir =  yDiff < 0 ? 2 : 3;
        }
        lastVia = via;
    }
}

bool DatastreamView::redrawStreams(bool addVia, QPoint newVia, bool keepViaBtns) {
    bool addedVia = false;
    if (!keepViaBtns) {
        deleteAllViaBtns();
    }

    int x = view->horizontalScrollBar()->value();
    int y = view->verticalScrollBar()->value();
    QPoint p(x, y);

    QGraphicsScene* scene = view->scene();
    scene->clear();

    for (std::list<PortOutButton*>::iterator i = portOuts.begin(); i != portOuts.end(); i++) {
        PortOut* p = (*i)->getPortOut();
        if (p->getDestination() != NULL) {
            QPen pen = QPen((*i)->getConnectionColor(), Qt::SolidLine);
            pen.setWidth(LINE_WIDTH);

            //first points: start and end
            list<pair<QPoint, bool>> viaList;
            QPoint p1 = (*i)->getPosition();
            QPoint p2 = ports[p->getDestination()]->getPosition();

            //dummy VIAs to ensure straight lines out of the modules
            int space1 = ((*i)->getPortSide()) ? CONNECTION_LINE_SPACE : -CONNECTION_LINE_SPACE;
            QPoint dummyVia1(p1.x() + space1, p1.y());
            int space2 = (ports[p->getDestination()]->getPortSide()) ? CONNECTION_LINE_SPACE : -CONNECTION_LINE_SPACE;
            QPoint dummyVia2(p2.x() + space2, p2.y());

            viaList.push_back(pair<QPoint, bool>(p1, false));
            viaList.push_back(pair<QPoint, bool>(dummyVia1, false));

            //add user defined VIAs to our list
            bool customViasPresent = addCustomVIAs(viaList, p, keepViaBtns, pen.color());

            //otherwise add pseudo mid-via
            if (!customViasPresent) {
                int xDiff = (dummyVia2.x()-dummyVia1.x())/2;
                int xmid = dummyVia1.x() + xDiff;
                int ymid;
                if ((*i)->getPortSide() == (xDiff < 0)) {
                    int y1 = (*i)->getModuleMiddle().y();
                    ymid =  y1 + ((ports[p->getDestination()]->getModuleMiddle().y() - y1) / 2);
                } else {
                    ymid = dummyVia1.y() + (dummyVia2.y()-dummyVia1.y())/2;
                }
                viaList.push_back(pair<QPoint, bool>(QPoint(xmid,ymid), false));
            }

            viaList.push_back(pair<QPoint, bool>(dummyVia2, false));
            viaList.push_back(pair<QPoint, bool>(p2, false));

            //make all connections either horizontal or vertical
            refineVIAs(viaList);

            //finally draw everything
            QPoint last = viaList.begin()->first;
            QPoint lastCustomVia = last;
            for (list<pair<QPoint, bool>>::iterator via = ++viaList.begin(); via != viaList.end(); via++) {
                QLine line(last, via->first);
                //if we are also searching for a place to add a VIA..
                if (addVia && !addedVia) {
                    if (checkOnLine(line, newVia)) {
                        //we found the right connection, add the VIA
                        p->addVia(newVia, lastCustomVia);
                        addedVia = true;
                    }
                }
                //draw the connection line part
                scene->addLine(line, pen);
                last = via->first;
                if (via->second) {
                    lastCustomVia = last;
                }
            }
        }
    }

    //bring module buttons to front (in front of VIAs)
    raiseModules();

    return addedVia;
}

void DatastreamView::raiseModules() {
    std::map<QWidget*, DatastreamObject*>::iterator i;
    for (i = moduleGuiElements.begin(); i != moduleGuiElements.end(); i++) {
        i->first->raise();
    }
}

void DatastreamView::moveDatastreamModules() {
    int x = view->horizontalScrollBar()->value();
    int y = view->verticalScrollBar()->value();
    QPoint p(x, y);

    std::map<QWidget*, DatastreamObject*>::iterator i;
    for (i = moduleGuiElements.begin(); i != moduleGuiElements.end(); i++) {
        DatastreamObject* dso = i->second;

        i->first->move(dso->getPosition() - p);
    }
}

void DatastreamView::configClickedModule() {
    QObject* sender = QObject::sender();
    if (sender != NULL) {
        try {
            QWidget* widget = dynamic_cast<QWidget*>(sender);
            DatastreamObject* object = moduleGuiElements.at(widget);
            emit requestConfigDialog(*object);
        } catch (exception) {
        }
    }
}

void DatastreamView::moduleRightClick(QPoint pos) {
    QObject* sender = QObject::sender();
    if (sender != NULL) {
        try {
            QWidget* widget = dynamic_cast<QWidget*>(sender);
            rightClickedObject = moduleGuiElements.at(widget);

            QMenu *menu = new QMenu(view);
            QAction* action = new QAction(QString::fromUtf8("Modul kopieren"), this);
            connect(action, SIGNAL(triggered(bool)), this, SLOT(copyModule()));
            menu->addAction(action);
            menu->popup(widget->mapToGlobal(pos));
        } catch (exception) {
        }
    }
}

void DatastreamView::viaMoved(QPoint oldPos, QPoint newPos) {
    QObject* sender = QObject::sender();
    if (sender != NULL) {
        try {
            QWidget* widget = dynamic_cast<QWidget*>(sender);
            PortOut* port = viaBtns.at(widget);

            int x = view->horizontalScrollBar()->value();
            int y = view->verticalScrollBar()->value();
            QPoint scrollPos(x, y);
            QPoint viaMid(VIA_WIDTH/2, VIA_WIDTH/2);
            port->moveVia(oldPos + viaMid + scrollPos, newPos + viaMid + scrollPos);
            redrawStreams(false, QPoint(0,0), true);
        } catch (exception) {
        }
    }
}

void DatastreamView::viaDelete() {
    QObject* sender = QObject::sender();
    if (sender != NULL) {
        try {
            QWidget* widget = dynamic_cast<QWidget*>(sender);
            PortOut* port = viaBtns.at(widget);

            int x = view->horizontalScrollBar()->value();
            int y = view->verticalScrollBar()->value();
            QPoint scrollPos(x, y);
            QPoint viaMid(VIA_WIDTH/2, VIA_WIDTH/2);
            port->deleteVia(widget->pos() + viaMid + scrollPos);
            redrawStreams();
        } catch (exception) {
        }
    }
}
