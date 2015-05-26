#include "newobjectdialog.h"
#include "ui_newobjectdialog.h"

using namespace std;

NewObjectDialog::NewObjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewObjectDialog)
{
    ui->setupUi(this);

    genrateTypeView();
}

NewObjectDialog::~NewObjectDialog()
{
    delete ui;
}

bool NewObjectDialog::generateItem(QStandardItem* parent, DataTypeStruct* dataType, bool recursive) {
    QStandardItem* item = new QStandardItem(QString(dataType->getDisplayName().c_str()));
    item->setEditable(false);
    item->setSelectable(dataType->isInstantiableObject());
    item->setToolTip(dataType->getDescription().c_str());
    item->setData(QVariant(dataType->getName().c_str()));
    if (recursive) {
        list<DataTypeStruct*> childs = dataType->getChilds();
        bool hasNotAbstractClass = dataType->isInstantiableObject();
        for (list<DataTypeStruct*>::iterator i = childs.begin(); i != childs.end(); i++) {
            hasNotAbstractClass |= generateItem(item, *i, true);
        }
        if (hasNotAbstractClass) {
            parent->appendRow(item);
        } else {
            delete item;
        }
        return hasNotAbstractClass;
    } else {
        parent->appendRow(item);
        return true;
    }
}

void NewObjectDialog::genrateTypeView() {
    QStandardItemModel* model = new QStandardItemModel();

    QStandardItem* device = new QStandardItem(QString::fromUtf8("Geräte"));
    device->setEditable(false);
    device->setSelectable(false);
    model->setItem(0, device);
    QStandardItem* dm = new QStandardItem(QString("Datastrom Module"));
    dm->setEditable(false);
    dm->setSelectable(false);
    model->setItem(1, dm);
    QStandardItem* sink = new QStandardItem(QString("Senken"));
    sink->setEditable(false);
    sink->setSelectable(false);
    model->setItem(2, sink);
    QStandardItem* other = new QStandardItem(QString("Andere"));
    other->setEditable(false);
    other->setSelectable(false);
    model->setItem(3, other);


    std::map<std::string, DataTypeStruct*> types = DataTypeStruct::getTypes();
    for (map<string, DataTypeStruct*>::iterator i = types.begin(); i != types.end(); i++) {
        DataTypeStruct* dt = i->second;
        if (dt->hasPrefix("device_")) {
            generateItem(device, i->second, false);
        } else if (dt->hasPrefix("dm_")) {
            generateItem(dm, i->second, false);
        } else if (dt->hasPrefix("sink_")) {
            generateItem(sink, i->second, false);
        } else {
            if (dt->getSuperType() == NULL) {
                if (dt->getName().compare("datastream_object_t") != 0) //filter out datastream_object_t since these types are covered by the other fields
                    generateItem(other, i->second, true);
            }
        }
    }

    model->setHorizontalHeaderItem( 0, new QStandardItem("Objekte"));
    ui->treeView->setModel(model);
}

DataTypeStruct* NewObjectDialog::getSelectedDataType() {
    QModelIndexList i = ui->treeView->selectionModel()->selectedIndexes();
    if (!i.isEmpty()) {
        QVariant v = i.first().data(Qt::UserRole + 1);
        try {
            return DataTypeStruct::getType(v.toString().toStdString());
        } catch (exception) {
            return NULL;
        }
    } else
        return NULL;
}
