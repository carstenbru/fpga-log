#ifndef NEWOBJECTDIALOG_H
#define NEWOBJECTDIALOG_H

#include <QDialog>
#include <map>
#include <QStandardItemModel>
#include "datatype.h"

namespace Ui {
class NewObjectDialog;
}

class NewObjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewObjectDialog(QWidget *parent, DataTypeStruct *type);
    ~NewObjectDialog();

    DataTypeStruct *getSelectedDataType();
private:
    void genrateTypeView(DataTypeStruct *type);
    bool generateItem(QStandardItem *parent, DataTypeStruct *dataType, bool recursive);

    Ui::NewObjectDialog *ui;
    int items;
    DataTypeStruct* lastInsertedType;
};

#endif // NEWOBJECTDIALOG_H
