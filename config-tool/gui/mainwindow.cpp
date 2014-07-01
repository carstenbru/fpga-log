#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "headerparser.h"
#include "newobjectdialog.h"
#include "configobjectdialog.h"
#include <iostream>
#include <QProcessEnvironment>
#include "consoleredirector.h"
#include "targetconfigdialog.h"
#include "outputgenerator.h"

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
    connect(ui->actionTarget, SIGNAL(triggered()), this, SLOT(targetConfig()));
    connect(ui->actionGenerate, SIGNAL(triggered()), this, SLOT(generate()));

    ui->listView->setModel(&otherModel);
    connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(otherObjectConfig(QModelIndex)));
}

void MainWindow::show() {
    QMainWindow::show();

    datastreamView = new DatastreamView(ui->graphicsView, &dataLogger);
    connect(datastreamView, SIGNAL(requestConfigDialog(CObject&)), this, SLOT(showConfigDialog(CObject&)));

    string spmc_root = QProcessEnvironment::systemEnvironment().value("SPARTANMC_ROOT").toStdString();
    if (spmc_root.empty()) {
        cerr << "SpartanMC root nicht gesetzt. Bitte setzen sie die $SPARTANMC_ROOT Umgebungsvariable." << endl;
    }

    dataLogger.newObject(DataTypeStruct::getType("device_hct99_t")); //TODO remove
    dataLogger.newObject(DataTypeStruct::getType("dm_timer_t"));
    dataLogger.newObject(DataTypeStruct::getType("sink_uart_t"));
    dataLogger.newObject(DataTypeStruct::getType("formatter_simple_t"));
    dataLogger.newObject(DataTypeStruct::getType("control_protocol_ascii_t"));
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
    if (dialog.exec() == QDialog::Rejected) {
        dataLogger.deleteObject(&object);
    }
}

void MainWindow::targetConfig() {
    TargetConfigDialog dialog(this, &dataLogger);
    dialog.exec();
}

void MainWindow::generate() {
    OutputGenerator og(&dataLogger);
    og.generateCSource();
}