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
    explicit NewObjectDialog(QWidget *parent);
    ~NewObjectDialog();

    DataType* getSelectedDataType();
private:
    void genrateTypeView();
    bool generateItem(QStandardItem *parent, DataType* dataType, bool recursive);

    Ui::NewObjectDialog *ui;
};

#endif // NEWOBJECTDIALOG_H
