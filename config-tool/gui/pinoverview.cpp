#include "pinoverview.h"
#include "ui_pinoverview.h"

#include "datatype.h"
#include <fstream>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>

using namespace std;

//TODO maybe direction could be interesting for user? other values maybe not..

PinOverview::PinOverview(QWidget *parent, DataLogger* dataLogger, string dataLoggerPath) :
    QDialog(parent),
    ui(new Ui::PinOverview),
    dataLogger(dataLogger),
    dataLoggerPath(dataLoggerPath)
{
    ui->setupUi(this);
    connect(ui->exportButton, SIGNAL(clicked(bool)), this, SLOT(exportOverview()));
    connect(ui->freePin, SIGNAL(clicked(bool)), this, SLOT(freePin()));
    connect(ui->assignPin, SIGNAL(clicked(bool)), this, SLOT(assignPin()));
    items.clear();

    QStandardItemModel* model = new QStandardItemModel();
    model->setHorizontalHeaderItem( 0, new QStandardItem(tr("Pin")));
    model->setHorizontalHeaderItem( 1, new QStandardItem(tr("Module")));
    model->setHorizontalHeaderItem( 2, new QStandardItem(tr("Group")));
    model->setHorizontalHeaderItem( 3, new QStandardItem(tr("Function")));

    int pos = 0;
    list<string> pinGroups = DataTypePin::getPinType()->getGroups();
    for (list<string>::iterator i = pinGroups.begin(); i != pinGroups.end(); i++) {
        QStandardItem* pinGroup = new QStandardItem(i->c_str());
        pinGroup->setEditable(false);
        pinGroup->setSelectable(false);
        for (int j = 1; j <= 3; j++) {
            QStandardItem* dummy = new QStandardItem();
            dummy->setEditable(false);
            dummy->setSelectable(false);
            model->setItem(pos,j,dummy);
        }
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
    pendingAssignmentsModel = new QStandardItemModel();
    pendingAssignmentsModel->setHorizontalHeaderItem( 0, new QStandardItem(tr("Module/Group")));
    pendingAssignmentsModel->setHorizontalHeaderItem( 1, new QStandardItem(tr("Function")));
    int pos = 0;
    QStandardItem* module;

    map<CParameter*, string[4]> pinAssignments = dataLogger->getPinAssignments();
    for (map<CParameter*, string[4]>::iterator pin = pinAssignments.begin(); pin != pinAssignments.end(); pin++) {
        string pinArray[4] = pin->second;

        pinParameters[pinArray[1] + pinArray[2] + pinArray[3]] = pin->first;

        string pinName = pinArray[0];
        bool exists = false;
        if (!pinName.empty()) {
            try {
                QStandardItem** itemPointers = items.at(pinName);
                itemPointers[1]->setText(pinArray[1].c_str());
                itemPointers[2]->setText(pinArray[2].c_str());
                itemPointers[3]->setText(pinArray[3].c_str());
                exists = true;
            } catch (exception) {

            }
        }
        if (!exists){
            try {
                module = pendingAssignmentGroups.at(pinArray[1]);
            } catch (exception) {
                module = new QStandardItem(pinArray[1].c_str());
                module->setEditable(false);
                module->setSelectable(false);
                QStandardItem* dummy = new QStandardItem();
                dummy->setEditable(false);
                dummy->setSelectable(false);

                pendingAssignmentsModel->setItem(pos, module);
                pendingAssignmentsModel->setItem(pos, 1, dummy);
                pos++;

                pendingAssignmentGroups[pinArray[1]] = module;
            }

            QStandardItem* group = new QStandardItem(pinArray[2].c_str());
            group->setEditable(false);
            group->setSelectable(true);

            QStandardItem* func = new QStandardItem(pinArray[3].c_str());
            func->setEditable(false);
            func->setSelectable(true);

            QList<QStandardItem*> row;
            row.append(group);
            row.append(func);

            module->appendRow(row);
        }
    }

    ui->pendingAssignments->setModel(pendingAssignmentsModel);
    ui->pendingAssignments->expandAll();
    ui->pendingAssignments->resizeColumnToContents(0);
    ui->pendingAssignments->resizeColumnToContents(1);
}

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
                           tr("Datalogger not saved"),
                           tr("The datalogger has to be saved first."),
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
    file << "<h1>" << tr("Project:").toStdString() << " " << name << "</h1>" << endl;
    file << tr("Target Platform:").toStdString() << " " << dataLogger->getTarget()->getValue() << endl;

    writeHTMLtable(file);

    file << "</body>" << endl << "</html>" << endl;
    file.close();

    QMessageBox dialog(QMessageBox::Information,
                       tr("Pinout exportet"),
                       tr("The pinout was exportet successfully to the file %1/pinout.html.").arg(path.c_str()),
                       QMessageBox::Ok);
    dialog.exec();
}

PinOverview::~PinOverview()
{
    delete ui;
}

void PinOverview::addPendingAssignment(string definition[3]) {
    QStandardItem* parent;
    try {
        parent = pendingAssignmentGroups.at(definition[0]);
    } catch (exception) {
        parent = new QStandardItem(definition[0].c_str());
        parent->setEditable(false);
        parent->setSelectable(false);

        QStandardItem* dummy = new QStandardItem();
        dummy->setEditable(false);
        dummy->setSelectable(false);

        QList<QStandardItem*> row;
        row.append(parent);
        row.append(dummy);
        pendingAssignmentsModel->appendRow(row);

        pendingAssignmentGroups[definition[0]] = parent;
    }

    QStandardItem* group = new QStandardItem(definition[1].c_str());
    group->setEditable(false);
    group->setSelectable(true);

    QStandardItem* func = new QStandardItem(definition[2].c_str());
    func->setEditable(false);
    func->setSelectable(true);

    QList<QStandardItem*> row;
    row.append(group);
    row.append(func);

    parent->appendRow(row);
    ui->pendingAssignments->expandAll();
    ui->pendingAssignments->resizeColumnToContents(0);
    ui->pendingAssignments->resizeColumnToContents(1);
}

void PinOverview::freePin() {
    QModelIndexList i = ui->pinOverview->selectionModel()->selectedIndexes();
    if (!i.isEmpty()) {
        QVariant v = i.first().data(Qt::DisplayRole);
        string pin = v.toString().toStdString();
        if (!pin.empty()) {
            string assignment;
            string definition[3];
            int c = 0;
            for (QModelIndexList::iterator p = ++i.begin(); p != i.end(); p++) {
                assignment = assignment + p->data(Qt::DisplayRole).toString().toStdString();
                definition[c++] = p->data(Qt::DisplayRole).toString().toStdString();
            }
            try {
                CParameter* param = pinParameters.at(assignment);
                param->setValue("");

                string completePinName = i.first().parent().data(Qt::DisplayRole).toString().toStdString() + ":" + pin;
                for (int j = 1; j <= 3; j++) {
                    items[completePinName][j]->setText("");
                }


                addPendingAssignment(definition);
            } catch (exception) {

            }
            return;
        }
    }
}

void PinOverview::assignPin() {
    QModelIndexList pi = ui->pinOverview->selectionModel()->selectedIndexes();
    if (!pi.isEmpty()) {
        QVariant v = pi.first().data(Qt::DisplayRole);
        string pin = v.toString().toStdString();
        if (!pin.empty()) { //sth selected in the pin overview
            QModelIndexList ai = ui->pendingAssignments->selectionModel()->selectedIndexes();
            if (!ai.isEmpty()) {
                QVariant v = ai.first().data(Qt::DisplayRole);
                string assignFunc = v.toString().toStdString();
                if (!assignFunc.empty()) { //sth selected in the pending pins box
                    string pinFree = (*(++pi.begin())).data(Qt::DisplayRole).toString().toStdString();
                    if (pinFree.empty()) { //pin currently not assigned
                        string completePinName = pi.first().parent().data(Qt::DisplayRole).toString().toStdString() + ":" + pin;
                        string module = ai.first().parent().data(Qt::DisplayRole).toString().toStdString();
                        string pinFunc = (++ai.begin())->data(Qt::DisplayRole).toString().toStdString();
                        string assignment = module + assignFunc + pinFunc;
                        try {
                            CParameter* param = pinParameters.at(assignment);
                            param->setValue(completePinName);
                        } catch (exception) {

                        }
                        items[completePinName][1]->setText(module.c_str());
                        items[completePinName][2]->setText(assignFunc.c_str());
                        items[completePinName][3]->setText(pinFunc.c_str());

                        pendingAssignmentsModel->removeRow(ai.first().row(), ai.first().parent());

                        if (!pendingAssignmentsModel->hasChildren(ai.first().parent())) {
                            pendingAssignmentsModel->removeRow(ai.first().parent().row());
                            pendingAssignmentGroups.erase(module);
                        }
                    }
                }
            }
            return;
        }
    }
}
