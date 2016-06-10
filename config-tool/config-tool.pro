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
        gui/mainwindow.cpp \
    datastreamobject.cpp \
    port.cpp \
    datalogger.cpp \
    gui/datastreamview.cpp \
    gui/moveablebutton.cpp \
    gui/portbutton.cpp \
    headerparser.cpp \
    datatype.cpp \
    gui/newobjectdialog.cpp \
    cmethod.cpp \
    cobject.cpp \
    gui/dataloggerothermodel.cpp \
    gui/consoleredirector.cpp \
    gui/configobjectdialog.cpp \
    spmcperipheral.cpp \
    cparameter.cpp \
    gui/pinbox.cpp \
    gui/targetconfigdialog.cpp \
    outputgenerator.cpp \
    gui/newmethoddialog.cpp \
    gui/pinoverview.cpp \
    automaticcoreassigner.cpp \
    gui/developertools.cpp \
    gui/acoparametersdialog.cpp

HEADERS  += gui/mainwindow.h \
    datastreamobject.h \
    port.h \
    datalogger.h \
    gui/datastreamview.h \
    gui/moveablebutton.h \
    gui/portbutton.h \
    headerparser.h \
    datatype.h \
    gui/newobjectdialog.h \
    cmethod.h \
    cobject.h \
    gui/dataloggerothermodel.h \
    gui/consoleredirector.h \
    gui/configobjectdialog.h \
    spmcperipheral.h \
    cparameter.h \
    gui/pinbox.h \
    gui/targetconfigdialog.h \
    outputgenerator.h \
    gui/newmethoddialog.h \
    gui/pinoverview.h \
    automaticcoreassigner.h \
    gui/developertools.h \
    gui/acoparametersdialog.h

FORMS    += gui/mainwindow.ui \
    gui/newobjectdialog.ui \
    gui/configobjectdialog.ui \
    gui/targetconfigdialog.ui \
    gui/newmethoddialog.ui \
    gui/pinoverview.ui \
    gui/developertools.ui \
    gui/acoparametersdialog.ui

RESOURCES += \
    icons.qrc
