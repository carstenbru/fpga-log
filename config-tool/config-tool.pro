#-------------------------------------------------
#
# Project created by QtCreator 2014-06-20T15:33:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = config-tool
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    datastreamobject.cpp \
    port.cpp \
    datalogger.cpp \
    datastreamview.cpp \
    moveablebutton.cpp \
    portbutton.cpp

HEADERS  += mainwindow.h \
    datastreamobject.h \
    port.h \
    datalogger.h \
    datastreamview.h \
    moveablebutton.h \
    portbutton.h

FORMS    += mainwindow.ui

RESOURCES += \
    icons.qrc
