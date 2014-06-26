#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "headerparser.h"
#include "newobjectdialog.h"
#include "configobjectdialog.h"
#include <iostream>
#include <QProcessEnvironment>
#include "consoleredirector.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    otherModel(&dataLogger)
{
    ui->setupUi(this);
    new ConsoleRedirector(cout, ui->textBrowser, "Black");
    new ConsoleRedirector(cerr, ui->textBrowser, "Red");

    connect(ui->actionNewObject, SIGNAL(triggered()), this, SLOT(newObject()));

    ui->listView->setModel(&otherModel);
    connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(otherObjectConfig(QModelIndex)));
}

void MainWindow::show() {
    QMainWindow::show();

    datastreamView = new DatastreamView(ui->graphicsView, &dataLogger);
    connect(datastreamView, SIGNAL(requestConfigDialog(CObject&)), this, SLOT(showConfigDialog(CObject&)));

    HeaderParser hp = HeaderParser(); //TODO move to main?
    string spmc_root = QProcessEnvironment::systemEnvironment().value("SPARTANMC_ROOT").toStdString();
    hp.addFolder(spmc_root + "/spartanmc/include/peripherals", false);
    hp.addFolder("../source/include", false);
    hp.addFolder("../source/include/device", true);
    hp.addFolder("../source/include/dm", true);
    hp.addFolder("../source/include/sink", true);
    hp.parseFiles();

    dataLogger.newObject(DataTypeStruct::getType("device_hct99_t")); //TODO remove
    dataLogger.newObject(DataTypeStruct::getType("sink_sd_card_t"));
    dataLogger.newObject(DataTypeStruct::getType("dm_splitter_data_t"));
    dataLogger.newObject(DataTypeStruct::getType("formatter_t"));
}

MainWindow::~MainWindow()
{
    delete datastreamView;
    delete ui;
}

void MainWindow::newObject() {
    NewObjectDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        DataTypeStruct* dataType = dialog.getSelectedDataType();
        if (dataType != NULL) {
            dataLogger.newObject(dataType);
        }
     }
}

void MainWindow::otherObjectConfig(QModelIndex index) {
    string objectName = index.data(Qt::UserRole + 1).toString().toStdString();
    vector<CObject*> objects = dataLogger.getOtherObjects();
    vector<CObject*>::iterator i;
    for (i = objects.begin(); i != objects.end(); i++) {
        if ((*i)->getName().compare(objectName) == 0)
            break;
    }

    showConfigDialog(**i);
}

void MainWindow::showConfigDialog(CObject& object) {
    ConfigObjectDialog dialog(this, &object, &dataLogger);
    dialog.exec();
}
