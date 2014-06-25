#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "headerparser.h"
#include "newobjectdialog.h"
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
    hp.addFolder(spmc_root + "/spartanmc/include/peripherals", false, true);
    hp.addFolder("../source/include", false, false);
    hp.addFolder("../source/include/device", true, false);
    hp.addFolder("../source/include/dm", true, false);
    hp.addFolder("../source/include/sink", true, false);
    hp.parseFiles();

    dataLogger.newObject(DataType::getType("device_hct99_t")); //TODO remove
    dataLogger.newObject(DataType::getType("sink_sd_card_t"));
    dataLogger.newObject(DataType::getType("dm_splitter_data_t"));
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
    cerr << "config called with object: " << object.getName() << endl;
    //TODO
}
