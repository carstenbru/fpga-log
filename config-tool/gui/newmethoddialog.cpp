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
    model->setHorizontalHeaderItem( 0, new QStandardItem(QString::fromUtf8("verfügbare Methoden:")));

    int pos = 0;
    for (list<CMethod*>::iterator i = methods.begin(); i != methods.end(); i++) {
        if (!(*i)->getHideFromUser()) {
            QStandardItem* item = new QStandardItem((*i)->getName().c_str());
            item->setEditable(false);
            item->setSelectable(true);
            item->setData((*i)->getDescription().c_str(), Qt::ToolTipRole);
            model->setItem(pos, item);

            pos++;
        }
    }

    ui->methodList->setModel(model);
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
