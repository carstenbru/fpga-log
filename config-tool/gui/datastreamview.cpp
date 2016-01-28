#include "datastreamview.h"
#include "moveablebutton.h"
#include <QScrollBar>
#include <QWidget>

using namespace std;

DatastreamView::DatastreamView(QGraphicsView* view) :
    view(view),
    dataLogger(NULL)
{
    view->setScene(new QGraphicsScene(QRectF(0, 0, 5300, 3005))); //TODO scene size

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

/*
class Via {
public:
    Via(int x, int y) : x(x), y(y) {}
    Via(QPoint point) : x(point.x()), y(point.y()) {}

    int getX() {return x;}
    int getY() {return x;}
private:
    int x;
    int y;
};*/

void DatastreamView::redrawStreams() {
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

            list<QPoint> viaList;
            QPoint p1 = (*i)->getPosition();
            QPoint p2 = ports[p->getDestination()]->getPosition();

            int space1 = ((*i)->getPortSide()) ? CONNECTION_LINE_SPACE : -CONNECTION_LINE_SPACE;
            QPoint dummyVia1(p1.x() + space1, p1.y());
            int space2 = (ports[p->getDestination()]->getPortSide()) ? CONNECTION_LINE_SPACE : -CONNECTION_LINE_SPACE;
            QPoint dummyVia2(p2.x() + space2, p2.y());

            viaList.push_back(p1);
            viaList.push_back(dummyVia1);

            //TODO add custom VIAs here

            //otherwise add pseudo mid-via
            int xDiff = (dummyVia2.x()-dummyVia1.x())/2;
            int xmid = dummyVia1.x() + xDiff;
            int ymid;
            if ((*i)->getPortSide() == (xDiff < 0)) {
                int y1 = (*i)->getModuleMiddle().y();
                ymid =  y1 + ((ports[p->getDestination()]->getModuleMiddle().y() - y1) / 2);
            } else {
                ymid = dummyVia1.y() + (dummyVia2.y()-dummyVia1.y())/2;
            }
            viaList.push_back(QPoint(xmid,ymid));

            viaList.push_back(dummyVia2);
            viaList.push_back(p2);

            list<QPoint>::iterator lastVia = viaList.begin();
            int lastDir = -1;
            for (list<QPoint>::iterator via = ++viaList.begin(); via != viaList.end(); via++) {
                int xDiff = via->x() - lastVia->x();
                int yDiff = via->y() - lastVia->y();

                if ((xDiff != 0) && (yDiff != 0)) {
                    int xDirInv = xDiff < 0 ? 1 : 0;
                    if ((lastDir >= 2) || (xDirInv == lastDir)) {
                        viaList.insert(via, QPoint(lastVia->x(), via->y()));
                        yDiff = 0;
                    } else {
                        viaList.insert(via, QPoint(via->x(), lastVia->y()));
                        xDiff = 0;
                    }
                }
                if (xDiff != 0) {
                    lastDir = xDiff < 0 ? 0 : 1;
                } else {
                    lastDir =  xDiff < 0 ? 2 : 3;
                }
                lastVia = via;
            }

            QPoint last = *viaList.begin();
            for (list<QPoint>::iterator via = ++viaList.begin(); via != viaList.end(); via++) {
                scene->addLine(QLine(last, *via), pen);
                last = *via;
            }
        }
    }

    //old drawing algorithm
    /*
    for (std::list<PortOutButton*>::iterator i = portOuts.begin(); i != portOuts.end(); i++) {
        PortOut* p = (*i)->getPortOut();
        if (p->getDestination() != NULL) {
            QPoint l1 = (*i)->getPosition();
            QPoint l2 = ports[p->getDestination()]->getPosition();
            QPen pen = QPen((*i)->getConnectionColor(), Qt::SolidLine);
            pen.setWidth(LINE_WIDTH);

            if ((*i)->getPortSide() != (l2.x()-l1.x() < 0)) {
                int xmid = l1.x() + ((l2.x() - l1.x()) / 2);
                QPoint l3(xmid, l1.y());
                scene->addLine(QLine(l1,l3), pen);
                QPoint l4(xmid, l2.y());
                scene->addLine(QLine(l3,l4),pen);
                scene->addLine(QLine(l4,l2),pen);
            } else {
                int y1 = (*i)->getModuleMiddle().y();
                int ymid =  y1 + ((ports[p->getDestination()]->getModuleMiddle().y() - y1) / 2);

                int space = ((*i)->getPortSide()) ? CONNECTION_LINE_SPACE : -CONNECTION_LINE_SPACE;
                QPoint l3(l1.x() + space, l1.y());
                scene->addLine(QLine(l1,l3), pen);
                QPoint l4(l1.x() + space, ymid);
                scene->addLine(QLine(l3,l4), pen);
                QPoint l5(l2.x() - space, ymid);
                scene->addLine(QLine(l4,l5), pen);
                QPoint l6(l2.x() - space, l2.y());
                scene->addLine(QLine(l5,l6), pen);
                scene->addLine(QLine(l6,l2), pen);
            }
        }
    }*/
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
