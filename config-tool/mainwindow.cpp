#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "headerparser.h"
#include "newobjectdialog.h"
#include <iostream>
#include <QProcessEnvironment>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionNewObject, SIGNAL(triggered()), this, SLOT(newObject()));
}

void MainWindow::show() {
    QMainWindow::show();

    datastreamView = new DatastreamView(ui->graphicsView, &dataLogger);

    HeaderParser hp = HeaderParser();
    string spmc_root = QProcessEnvironment::systemEnvironment().value("SPARTANMC_ROOT").toStdString();
    hp.addFolder(spmc_root + "/spartanmc/include/peripherals", false, true);
    hp.addFolder("../source/include", false, false);
    hp.addFolder("../source/include/device", true, false);
    hp.addFolder("../source/include/dm", true, false);
    hp.addFolder("../source/include/sink", true, false);
    hp.parseFiles();
}

MainWindow::~MainWindow()
{
    delete datastreamView;
    delete ui;
}

void MainWindow::newObject() {
    NewObjectDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        DataType* dataType = dialog.getSelectedDataType();
        if (dataType != NULL) {
            if (dataType->hasPrefix("device_") || dataType->hasPrefix("dm_") || dataType->hasPrefix("sink_"))
                dataLogger.newDatastreamObject(dataType);
            else {
                cout << dataType->getDisplayName() << endl;
//TODO
            }
        }
     }
}
