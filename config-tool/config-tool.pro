#-------------------------------------------------
#
# Project created by QtCreator 2014-06-20T15:33:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = config-tool
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    datastreamobject.cpp \
    port.cpp \
    datalogger.cpp \
    datastreamview.cpp \
    moveablebutton.cpp \
    portbutton.cpp \
    headerparser.cpp \
    datatype.cpp \
    newobjectdialog.cpp \
    cmethod.cpp \
    cobject.cpp \
    dataloggerothermodel.cpp

HEADERS  += mainwindow.h \
    datastreamobject.h \
    port.h \
    datalogger.h \
    datastreamview.h \
    moveablebutton.h \
    portbutton.h \
    headerparser.h \
    datatype.h \
    newobjectdialog.h \
    cmethod.h \
    cobject.h \
    dataloggerothermodel.h \
    Q_DebugStream.h

FORMS    += mainwindow.ui \
    newobjectdialog.ui

RESOURCES += \
    icons.qrc
