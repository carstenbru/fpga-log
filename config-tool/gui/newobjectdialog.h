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
    /**
     * @brief generates the view of selectable types
     *
     * @param type type of which the object should be a subclass, pass NULL for to select from all types
     */
    void genrateTypeView(DataTypeStruct *type);
    /**
     * @brief generates one item for a single type in the type view
     *
     * @param parent parent item
     * @param dataType data type for which the item should be created
     * @param recursive true to also add child types of this type
     * @return true if this class or any subclass is not abstract (i.e. it really should be added to the view)
     */
    bool generateItem(QStandardItem *parent, DataTypeStruct *dataType, bool recursive);

    Ui::NewObjectDialog *ui; /**< UI */
    int items; /**< number of items in the view */
    DataTypeStruct* lastInsertedType; /**< last type that was inserted to the type view */
private slots:
    /**
     * @brief slot to notify about a double click on an item
     *
     * @param index index of the clicked item
     */
    void itemDoubleClicked(QModelIndex index);
};

#endif // NEWOBJECTDIALOG_H
