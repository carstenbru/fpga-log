#include "pinoverview.h"
#include "ui_pinoverview.h"

#include "datatype.h"
#include <fstream>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>

using namespace std;

//TODO not assigned pins (from modules)!!!
//TODO assign pins (boxe woth not assigned pins, exchange)
//TODO maybe direction could be interesting for user? other values maybe not..
//TODO correct after target change and Pin assigment orphaned??

PinOverview::PinOverview(QWidget *parent, DataLogger* dataLogger, string dataLoggerPath) :
    QDialog(parent),
    ui(new Ui::PinOverview),
    dataLogger(dataLogger),
    dataLoggerPath(dataLoggerPath)
{
    ui->setupUi(this);
    connect(ui->exportButton, SIGNAL(clicked(bool)), this, SLOT(exportOverview()));
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

#include <iostream>
void PinOverview::writeHTMLtable(ofstream& file) {
    file << "<table border= \"1\">" << endl;

    file << "<tr>" << endl;
    file << "<th>Pin</th>" << endl;
    file << "<th>Modul</th>" << endl;
    file << "<th>Gruppe</th>" << endl;
    file << "<th>Funktion</th>" << endl;
    file << "</tr>" << endl;

    string lastGroup;
    for (map<string, QStandardItem*[4]>::iterator i = items.begin(); i != items.end(); i++) {
        string group = Pin::getGroupFromFullName(i->first);
        if (group.compare(lastGroup) != 0) {
            file << "<tr>" << endl;
            file << "<td colspan=\"4\"><b>" << group << "</b></td>" << endl;
            file << "</tr>" << endl;
            lastGroup = group;
        }

        QStandardItem** items = i->second;
        file << "<tr>" << endl;
        for (int j = 0; j < 4; j++) {
          file << "<td>" << items[j]->text().toStdString() << "</td>" << endl;
        }
        file << "</tr>" << endl;
    }

    file << "</table>" << endl;
}

void PinOverview::exportOverview() {
    if (dataLoggerPath.empty()) {
        QMessageBox dialog(QMessageBox::Critical,
                           "Datenlogger nicht gespeichert",
                           "Der Datenlogger muss zuerst gespeichert werden.",
                           QMessageBox::Ok);
        dialog.exec();
        return;
    }

    string path = QFileInfo(dataLoggerPath.c_str()).dir().absolutePath().toStdString() + "/doc";
    QDir().mkpath(path.c_str());
    ofstream file;
    file.open(path + "/pinout.html");

    file << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN" << endl <<
            "http://www.w3.org/TR/html4/loose.dtd\">" << endl;
    file << "<html>" << endl << "<head>" << endl << "<title>fpga-log</title>" << endl;
    file << "</head>" << endl << "<body>" << endl;

    string name = dataLoggerPath.erase(0, dataLoggerPath.rfind("/")+1);
    name.erase(dataLoggerPath.rfind("."), dataLoggerPath.length());
    file << "<h1>" << "Projekt: " << name << "</h1>" << endl;
    file << "Zielplattform: " << dataLogger->getTarget()->getValue() << endl;

    writeHTMLtable(file);

    file << "</body>" << endl << "</html>" << endl;
    file.close();

    QMessageBox dialog(QMessageBox::Information,
                       "Pinbelegung exportiert",
                       ("Die Pinbelegung wurde erfolgreich in die Datei " + path + "/pinout.html exportiert.").c_str(),
                       QMessageBox::Ok);
    dialog.exec();
}

PinOverview::~PinOverview()
{
    delete ui;
}
