#include "port.h"

int ControlPortIn::connectPort(Port *port) {
    ControlPortOut* d = dynamic_cast<ControlPortOut*>(port);
    if (d != NULL) {
        return d->connectPort(this);
    } else {
        return 0;
    }
}

int ControlPortOut::connectPort(Port *port) {
    ControlPortIn* d = dynamic_cast<ControlPortIn*>(port);
    if (d != NULL) {
        destination = d;
        emit connected();
        return 1;
    } else {
        return 0;
    }
}

int DataPortIn::connectPort(Port *port) {
    DataPortOut* d = dynamic_cast<DataPortOut*>(port);
    if (d != NULL) {
        return d->connectPort(this);
    } else {
        return 0;
    }
}

int DataPortOut::connectPort(Port *port) {
    DataPortIn* d = dynamic_cast<DataPortIn*>(port);
    if (d != NULL) {
        destination = d;
        emit connected();
        return 1;
    } else {
        return 0;
    }
}

void PortOut::disconnectPort() {
    destination = NULL;
    emit disconnected();
}
