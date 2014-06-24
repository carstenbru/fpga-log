#include "mainwindow.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    new DataType("void", true); //TODO move
    new DataType("int", true);
    new DataType("unsigned int", true);
    new DataType("unsigned char", true);
    new DataType("uint36_t", true);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
