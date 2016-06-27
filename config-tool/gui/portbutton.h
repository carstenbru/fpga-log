/**
 * @file portbutton.h
 * @brief PortButton and inherited PortOutButton, ControlPortInButton, DataPortInButton, ControlPortOutButton and DataPortOutButton classes
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef PORTBUTTON_H
#define PORTBUTTON_H

#include <QObject>
#include <QPushButton>
#include <QMouseEvent>
#include "port.h"
#include "datastreamobject.h"

//y-postion of the first port button
#define PORT_BUTTON_Y_TOP 10
//y-spacing between port buttons
#define PORT_BUTTON_Y_SPACE 20
//x-overlap of port buttons (overlap with the module button)
#define PORT_BUTTON_X_OVERLAP 5
//button size in x-direction
#define PORT_BUTTON_SIZE_X 30
//button size in y-direction
#define PORT_BUTTON_SIZE_Y 30

//color of a data stream button
#define COLOR_DATASTREAM (Qt::red)
//color of a control stream button
#define COLOR_CONTROLSTREAM (QColor(34,185,165))

/**
 * @brief class implementing a button for a port
 */
class PortButton : public QPushButton
{
    Q_OBJECT

public:
    /**
     * @brief constructor
     *
     * @param parent parent widget
     * @param port port the button represents
     * @param dso object the port is part of
     */
    PortButton(QWidget* parent, Port *port, DatastreamObject* dso);
    virtual ~PortButton() {}

    /**
     * @brief gets the buttons position
     *
     * @return the buttons position
     */
    virtual QPoint getPosition() = 0;

    /**
     * @brief gets the color of a connection attached to this port
     * @return
     */
    virtual QColor getConnectionColor() = 0;

    /**
     * @brief gets the side of the module at which the button is drawn (0 for left, 1 for right)
     *
     * @return the side of the module at which the button is drawn
     */
    virtual int getPortSide() = 0;

    /**
     * @brief gets the middle of the ports parent module button
     *
     * @return gets the middle of the ports parent module button
     */
    QPoint getModuleMiddle();
protected:
    Port* port; /**< the port the button represents */

    DatastreamObject* dso; /**< the datastream object this port belongs to */
private slots:
    void newConnection();
    void mousePressEvent(QMouseEvent *e);
};

/**
 * @brief class implementing a button for an output port
 */
class PortOutButton : public PortButton
{
public:
    /**
     * @brief constructor
     *
     * @param parent parent widget
     * @param port port the button is assigned to
     * @param dso datastream object of the port
     */
    PortOutButton(QWidget* parent, PortOut* port, DatastreamObject* dso);
    virtual ~PortOutButton() {}

    /**
     * @brief gets the output port which this button represents
     *
     * @return the output port which this button represents
     */
    PortOut* getPortOut() { return out_port; }
protected:
    PortOut* out_port; /**< the output port which this button represents */
};

/**
 * @brief class implementing a button for a control input port
 */
class ControlPortInButton : public PortButton
{
public:
    /**
     * @brief constructor
     *
     * @param parent parent widget
     * @param port port the button is assigned to
     * @param dso datastream object of the port
     * @param number number of this button, i.e. how many other port buttons are before this one
     */
    ControlPortInButton(QWidget* parent, ControlPortIn* port, DatastreamObject* dso, int number);
    virtual ~ControlPortInButton() {}

    virtual QPoint getPosition() { return dso->getPosition() + pos() + QPoint(0, PORT_BUTTON_SIZE_Y/2); }
    virtual QColor getConnectionColor() { return COLOR_CONTROLSTREAM; }
    virtual int getPortSide() { return 0; }
protected:
    ControlPortIn* cin_port; /**< the control input port which this button represents */
};

/**
 * @brief class implementing a button for a data input port
 */
class DataPortInButton : public PortButton
{
public:
    /**
     * @brief constructor
     *
     * @param parent parent widget
     * @param port port the button is assigned to
     * @param dso datastream object of the port
     * @param number number of this button, i.e. how many other port buttons are before this one
     */
    DataPortInButton(QWidget* parent, DataPortIn* port, DatastreamObject* dso, int number);
    virtual ~DataPortInButton() {}

    virtual QPoint getPosition() { return dso->getPosition() + pos() + QPoint(PORT_BUTTON_SIZE_X-PORT_BUTTON_X_OVERLAP, PORT_BUTTON_SIZE_Y/2); }
    virtual QColor getConnectionColor() { return COLOR_DATASTREAM; }
    virtual int getPortSide() { return 1; }
protected:
    DataPortIn* din_port; /**< the data input port which this button represents */
};

/**
 * @brief class implementing a button for a control output port
 */
class ControlPortOutButton : public PortOutButton
{
public:
    /**
     * @brief constructor
     *
     * @param parent parent widget
     * @param port port the button is assigned to
     * @param dso datastream object of the port
     * @param number number of this button, i.e. how many other port buttons are before this one
     */
    ControlPortOutButton(QWidget* parent, ControlPortOut* port, DatastreamObject* dso, int number);
    virtual ~ControlPortOutButton() {}

    virtual QPoint getPosition() { return dso->getPosition() + pos() + QPoint(PORT_BUTTON_SIZE_X-PORT_BUTTON_X_OVERLAP, PORT_BUTTON_SIZE_Y/2); }
    virtual QColor getConnectionColor() { return COLOR_CONTROLSTREAM; }
    virtual int getPortSide() { return 1; }
protected:
    ControlPortOut* cout_port; /**< the control output port which this button represents */
};

/**
 * @brief class implementing a button for a data output port
 */
class DataPortOutButton : public PortOutButton
{
public:
    /**
     * @brief constructor
     *
     * @param parent parent widget
     * @param port port the button is assigned to
     * @param dso datastream object of the port
     * @param number number of this button, i.e. how many other port buttons are before this one
     */
    DataPortOutButton(QWidget* parent, DataPortOut* port, DatastreamObject* dso, int number);
    virtual ~DataPortOutButton() {}

    virtual QPoint getPosition() { return dso->getPosition() + pos() + QPoint(0, PORT_BUTTON_SIZE_Y/2); }
    virtual QColor getConnectionColor() { return COLOR_DATASTREAM; }
    virtual int getPortSide() { return 0; }
protected:
    DataPortOut* dout_port; /**< the data output port which this button represents */
};

#endif // PORTBUTTON_H
