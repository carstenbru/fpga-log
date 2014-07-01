#include "portbutton.h"

PortButton::PortButton(QWidget* parent, Port *port, DatastreamObject *dso) :
    QPushButton(parent),
    port(port),
    dso(dso)
{
    setFlat(true);
    setToolTip(QString::fromStdString(port->getName()));
    show();

    connect(this, SIGNAL(clicked()), this, SLOT(newConnection()));
}

void PortButton::mousePressEvent(QMouseEvent *e) {
    if (e->buttons() & Qt::RightButton) {
        port->disconnectPort();
    }

    QPushButton::mousePressEvent(e);
}

void PortButton::newConnection() {
    static PortButton* lastClickedPort = NULL;

    if (lastClickedPort != NULL) {
        if (port->connectPort(lastClickedPort->port)) {
             lastClickedPort = NULL;
        } else {
            lastClickedPort = this;
        }
    } else {
        lastClickedPort = this;
    }
}

QPoint PortButton::getModuleMiddle() {
    return dso->getPosition() + QPoint(parentWidget()->size().width() / 2, parentWidget()->size().height() / 2);
}

PortOutButton::PortOutButton(QWidget* parent, PortOut* port, DatastreamObject* dso) :
    PortButton(parent, port, dso),
    out_port(port)
{

}

ControlPortInButton::ControlPortInButton(QWidget* parent, ControlPortIn* port, DatastreamObject* dso, int number) :
    PortButton(parent, port, dso),
    cin_port(port)
{
    setGeometry(-PORT_BUTTON_X_OVERLAP,
                       PORT_BUTTON_Y_TOP + number * (PORT_BUTTON_Y_SPACE + PORT_BUTTON_SIZE_Y),
                       PORT_BUTTON_SIZE_X,
                       PORT_BUTTON_SIZE_Y);
    setIcon(QIcon(":/icons/control_arrow"));
}

DataPortInButton::DataPortInButton(QWidget* parent, DataPortIn* port, DatastreamObject* dso, int number) :
    PortButton(parent, port, dso),
    din_port(port)
{
    setGeometry(parent->width() + PORT_BUTTON_X_OVERLAP - PORT_BUTTON_SIZE_X,
                       PORT_BUTTON_Y_TOP + number * (PORT_BUTTON_Y_SPACE + PORT_BUTTON_SIZE_Y),
                       PORT_BUTTON_SIZE_X,
                       PORT_BUTTON_SIZE_Y);
    setIcon(QIcon(":/icons/data_arrow"));
}

ControlPortOutButton::ControlPortOutButton(QWidget* parent, ControlPortOut* port, DatastreamObject* dso, int number) :
    PortOutButton(parent, port, dso),
    cout_port(port)
{
    setGeometry(parent->width() + PORT_BUTTON_X_OVERLAP - PORT_BUTTON_SIZE_X,
                       PORT_BUTTON_Y_TOP + number * (PORT_BUTTON_Y_SPACE + PORT_BUTTON_SIZE_Y),
                       PORT_BUTTON_SIZE_X,
                       PORT_BUTTON_SIZE_Y);
    setIcon(QIcon(":/icons/control_arrow"));
}

DataPortOutButton::DataPortOutButton(QWidget* parent, DataPortOut* port, DatastreamObject* dso, int number) :
    PortOutButton(parent, port, dso),
    dout_port(port)
{
    setGeometry(-PORT_BUTTON_X_OVERLAP,
                       PORT_BUTTON_Y_TOP + number * (PORT_BUTTON_Y_SPACE + PORT_BUTTON_SIZE_Y),
                       PORT_BUTTON_SIZE_X,
                       PORT_BUTTON_SIZE_Y);
    setIcon(QIcon(":/icons/data_arrow"));
}
