/**
 * @file newobjectdialog.h
 * @brief NewObjectDialog GUI class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef NEWOBJECTDIALOG_H
#define NEWOBJECTDIALOG_H

#include <QDialog>
#include <map>
#include <QStandardItemModel>
#include "datatype.h"

namespace Ui {
class NewObjectDialog;
}

/**
 * @brief dialog to select a type of a new object
 */
class NewObjectDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief constructor
     *
     * @param parent parent widget
     * @param type type of which the object should be a subclass, pass NULL for to select from all types
     */
    explicit NewObjectDialog(QWidget *parent, DataTypeStruct *type);
    ~NewObjectDialog();

    /**
     * @brief gets the data type selected by the user
     *
     * @return the data type selected by the user
     */
    DataTypeStruct *getSelectedDataType();
private:
    void genrateTypeView(DataTypeStruct *type);
    bool generateItem(QStandardItem *parent, DataTypeStruct *dataType, bool recursive);

    Ui::NewObjectDialog *ui;
    int items;
    DataTypeStruct* lastInsertedType;
private slots:
    void itemDoubleClicke(QModelIndex index);
};

#endif // NEWOBJECTDIALOG_H
