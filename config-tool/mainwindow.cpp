#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

DataLogger* d2;

void MainWindow::show() {
    QMainWindow::show();

    DataLogger* d = new DataLogger();
    d->newDatastreamObject("pwm");
    d->newDatastreamObject("trigger");
    datastreamView = new DatastreamView(ui->graphicsView, d); //TODO where generate DataLogger instance? delete!

    d2 = d;
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(test()));
}

MainWindow::~MainWindow()
{
    delete datastreamView;
    delete ui;
}

void MainWindow::test() {
    d2->newDatastreamObject("new");
}
