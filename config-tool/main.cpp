/**
 * @file main.cpp
 * @brief config-tool main file
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include "gui/mainwindow.h"
#include <QApplication>
#include <iostream>
#include <QProcessEnvironment>
#include <cmath>
#include "headerparser.h"
#include "automaticcoreassigner.h"

using namespace std;

/**
 * @brief creates the general data types used later in the program (e.g. for datalogger parameters)
 */
void createDataTypes() {
    new DataType("void", true);
    new DataTypeBoolean("bool", true);
    new DataTypeChar("char", true);
    new DataTypeString("string", true);
    new DataTypeNumber("int", -131072, 131071, true);
    new DataTypeNumber("long", -2147483648, 2147483647, true);
    new DataTypeNumber("unsigned int", 0, 262143, true);
    new DataTypeNumber("unsigned char", 0, 511, true);
    new DataTypeNumber("uint_least8_t", 0, 255, true);
    new DataTypeNumber("uint36_t", 0, 68719476735, true);
    new DataTypeNumber("int36_t", -34359738368, 34359738367, true);
    new DataTypeNumber("peripheral_int", -2147483648, 2147483647, true);
    new DataTypeFloat("coefficient_t", -9.9999, 9.9999, 4, true, true);

}

/**
 * @brief main program main function
 * @param argc argument count
 * @param argv commmand line arguments
 * @return program result code
 */
int main(int argc, char *argv[]) {
    DataLogger::loadTragetXMLs();
    SpmcPeripheral::loadPeripheralXMLs();

    createDataTypes();

    //create a dummy peripheral of dummy type sysclk_regs_t to force loading of clock divider and multiply data types
    new DataType("sysclk_regs_t", true);
    SpmcPeripheral("clk_dummy", DataType::getType("sysclk_regs_t"), NULL, NULL);


    //parse all header files to learn the fpga-log modules and other properties
    HeaderParser hp = HeaderParser(true);
    string spmc_root = QProcessEnvironment::systemEnvironment().value("SPARTANMC_ROOT").toStdString();
    if (!spmc_root.empty()) {
        hp.addFolder(spmc_root + "/spartanmc/include/peripherals", false);
        hp.addFolder("../source/include", false);
        hp.addFolder("../source/include/device", true);
        hp.addFolder("../source/include/dm", true);
        hp.addFolder("../source/include/sink", true);
        hp.parseFiles();
    }

    //read in module weights used for automatic core assignment
    AutomaticCoreAssigner aca;
    aca.readModuleWeights();

    //start GUI
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
