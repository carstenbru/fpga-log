#ifndef PORTBUTTON_H
#define PORTBUTTON_H

#include <QObject>
#include <QPushButton>
#include <QMouseEvent>
#include "port.h"
#include "datastreamobject.h"

#define PORT_BUTTON_Y_TOP 10
#define PORT_BUTTON_Y_SPACE 20
#define PORT_BUTTON_X_OVERLAP 5
#define PORT_BUTTON_SIZE_X 30
#define PORT_BUTTON_SIZE_Y 30

#define COLOR_DATASTREAM (Qt::red)
#define COLOR_CONTROLSTREAM (QColor(34,185,165))

class PortButton : public QPushButton
{
    Q_OBJECT

public:
    PortButton(QWidget* parent, Port *port, DatastreamObject* dso);
    virtual ~PortButton() {}

    virtual QPoint getPosition() = 0;
    virtual QColor getConnectionColor() = 0;
    virtual int getPortSide() = 0;
    QPoint getModuleMiddle();
protected:
    //QPushButton button;

    Port* port;

    DatastreamObject* dso;
private slots:
    void newConnection();
    void mousePressEvent(QMouseEvent *e);
};

class PortOutButton : public PortButton
{
public:
    PortOutButton(QWidget* parent, PortOut* port, DatastreamObject* dso);
    virtual ~PortOutButton() {}

    PortOut* getPortOut() { return out_port; }
protected:
    PortOut* out_port;
};

class ControlPortInButton : public PortButton
{
public:
    ControlPortInButton(QWidget* parent, ControlPortIn* port, DatastreamObject* dso, int number);
    virtual ~ControlPortInButton() {}

    virtual QPoint getPosition() { return dso->getPosition() + pos() + QPoint(0, PORT_BUTTON_SIZE_Y/2); }
    virtual QColor getConnectionColor() { return COLOR_CONTROLSTREAM; }
    virtual int getPortSide() { return 0; }
protected:
    ControlPortIn* cin_port;
};

class DataPortInButton : public PortButton
{
public:
    DataPortInButton(QWidget* parent, DataPortIn* port, DatastreamObject* dso, int number);
    virtual ~DataPortInButton() {}

    virtual QPoint getPosition() { return dso->getPosition() + pos() + QPoint(PORT_BUTTON_SIZE_X-PORT_BUTTON_X_OVERLAP, PORT_BUTTON_SIZE_Y/2); }
    virtual QColor getConnectionColor() { return COLOR_DATASTREAM; }
    virtual int getPortSide() { return 1; }
protected:
    DataPortIn* din_port;
};

class ControlPortOutButton : public PortOutButton
{
public:
    ControlPortOutButton(QWidget* parent, ControlPortOut* port, DatastreamObject* dso, int number);
    virtual ~ControlPortOutButton() {}

    virtual QPoint getPosition() { return dso->getPosition() + pos() + QPoint(PORT_BUTTON_SIZE_X-PORT_BUTTON_X_OVERLAP, PORT_BUTTON_SIZE_Y/2); }
    virtual QColor getConnectionColor() { return COLOR_CONTROLSTREAM; }
    virtual int getPortSide() { return 1; }
protected:
    ControlPortOut* cout_port;
};

class DataPortOutButton : public PortOutButton
{
public:
    DataPortOutButton(QWidget* parent, DataPortOut* port, DatastreamObject* dso, int number);
    virtual ~DataPortOutButton() {}

    virtual QPoint getPosition() { return dso->getPosition() + pos() + QPoint(0, PORT_BUTTON_SIZE_Y/2); }
    virtual QColor getConnectionColor() { return COLOR_DATASTREAM; }
    virtual int getPortSide() { return 0; }
protected:
    DataPortOut* dout_port;
};

#endif // PORTBUTTON_H
