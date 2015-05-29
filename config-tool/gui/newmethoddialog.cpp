#include "newmethoddialog.h"
#include "ui_newmethoddialog.h"

#include <QStandardItemModel>

using namespace std;

NewMethodDialog::NewMethodDialog(std::list<CMethod *> methods, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewMethodDialog)
{
    ui->setupUi(this);

    QStandardItemModel* model = new QStandardItemModel();
    model->setHorizontalHeaderItem( 0, new QStandardItem(QString::fromUtf8("Methodenname")));
    model->setHorizontalHeaderItem( 1, new QStandardItem(QString::fromUtf8("Beschreibung")));

    int pos = 0;
    for (list<CMethod*>::iterator i = methods.begin(); i != methods.end(); i++) {
        if (!(*i)->getHideFromUser()) {
            QStandardItem* item = new QStandardItem((*i)->getName().c_str());
            item->setEditable(false);
            item->setSelectable(true);
            model->setItem(pos, item);

            QStandardItem* item2 = new QStandardItem((*i)->getDescription().c_str());
            item2->setEditable(false);
            item2->setSelectable(true);
            model->setItem(pos, 1, item2);

            pos++;
        }
    }

    ui->methodList->setModel(model);
    ui->methodList->resizeColumnToContents(0);
    ui->methodList->resizeColumnToContents(1);

    QObject::connect(ui->methodList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(accept()));
}

string NewMethodDialog::getSelectedMethod() {
    QModelIndexList i = ui->methodList->selectionModel()->selectedIndexes();
    if (!i.isEmpty()) {
        QVariant v = i.first().data(Qt::DisplayRole);
        return v.toString().toStdString();
    } else
        return "";
}

NewMethodDialog::~NewMethodDialog()
{
    delete ui;
}
