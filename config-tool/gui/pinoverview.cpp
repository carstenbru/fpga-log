#include "pinoverview.h"
#include "ui_pinoverview.h"

#include "datatype.h"

using namespace std;

//TODO export overview (txt/html) (include datalogger name and target)
//TODO not assigned pins (from modules)!!!
//TODO assign pins (boxe woth not assigned pins, exchange)
//TODO maybe direction could be interesting for user? other values maybe not..
//TODO correct after target change and Pin assigment orphaned??

PinOverview::PinOverview(QWidget *parent, DataLogger* dataLogger) :
    QDialog(parent),
    ui(new Ui::PinOverview),
    dataLogger(dataLogger)
{
    ui->setupUi(this);
    items.clear();

    QStandardItemModel* model = new QStandardItemModel();
    model->setHorizontalHeaderItem( 0, new QStandardItem(QString::fromUtf8("Pin")));
    model->setHorizontalHeaderItem( 1, new QStandardItem(QString::fromUtf8("Modul")));
    model->setHorizontalHeaderItem( 2, new QStandardItem(QString::fromUtf8("Gruppe")));
    model->setHorizontalHeaderItem( 3, new QStandardItem(QString::fromUtf8("Funktion")));

    int pos = 0;
    list<string> pinGroups = DataTypePin::getPinType()->getGroups();
    for (list<string>::iterator i = pinGroups.begin(); i != pinGroups.end(); i++) {
        QStandardItem* pinGroup = new QStandardItem(i->c_str());
        pinGroup->setEditable(false);
        pinGroup->setSelectable(false);
        model->setItem(pos, pinGroup);

        list<Pin> pins = DataTypePin::getPinType()->getPinsInGroup(*i);
        for (list<Pin>::iterator p = pins.begin(); p != pins.end(); p++) {
            QStandardItem* pin = new QStandardItem(p->getName().c_str());
            pin->setEditable(false);
            pin->setSelectable(true);
            QList<QStandardItem*> row;
            row.append(pin);

            items[*i + ":" + p->getName()][0] = pin;

            for (int j = 1; j <= 3; j++) {
                QStandardItem* it = new QStandardItem();
                it->setEditable(false);
                it->setSelectable(true);
                row.append(it);

                items[*i + ":" + p->getName()][j] = it;
            }
            pinGroup->appendRow(row);
        }

        pos++;
    }

    readPinAssignments();

    ui->pinOverview->setModel(model);
    ui->pinOverview->expandAll();
    ui->pinOverview->resizeColumnToContents(0);
    ui->pinOverview->resizeColumnToContents(1);
    ui->pinOverview->resizeColumnToContents(2);
    ui->pinOverview->resizeColumnToContents(3);
    ui->pinOverview->collapseAll();
}

void PinOverview::readPinAssignments() {
    list<string[4]> pinAssignments = dataLogger->getPinAssignments();
    for (list<string[4]>::iterator pin = pinAssignments.begin(); pin != pinAssignments.end(); pin++) {
        string pinArray[4] = *pin;
        string pinName = pinArray[0];
        if (!pinName.empty()) {
            items[pinName][1]->setText(pinArray[1].c_str());
            items[pinName][2]->setText(pinArray[2].c_str());
            items[pinName][3]->setText(pinArray[3].c_str());
        } else {
            //TODO
        }
    }
}

PinOverview::~PinOverview()
{
    delete ui;
}
