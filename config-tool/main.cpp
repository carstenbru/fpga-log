#include "mainwindow.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    new DataType("void"); //TODO move, perhaps read from config file
    new DataTypeNumber("int", -131072, 131071);
    new DataTypeNumber("char", -256, 255);
    new DataTypeNumber("unsigned int", 0, 262143);
    new DataTypeNumber("unsigned char", 0, 511);
    new DataTypeNumber("uint36_t", 0, 68719476735);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
