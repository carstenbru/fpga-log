#include "mainwindow.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    new DataType("void"); //TODO move
    new DataType("int");
    new DataType("char");
    new DataType("unsigned int");
    new DataType("unsigned char");
    new DataType("uint36_t");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
