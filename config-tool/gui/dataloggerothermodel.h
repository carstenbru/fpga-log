/**
 * @file dataloggerothermodel.h
 * @brief DataLoggerOtherModel class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef DATALOGGEROTHERMODEL_H
#define DATALOGGEROTHERMODEL_H

#include <QAbstractListModel>
#include "datalogger.h"


/**
 * @brief The data model for non-datastream objects
 *
 * This model class is required to show the data corerctly in the QListView for objects
 */
class DataLoggerOtherModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /**
     * @brief constructor
     * @param parent parent object
     */
    explicit DataLoggerOtherModel(QObject *parent = 0);

    /**
     * @brief gets the number of rows in the model
     *
     * @return the number of rows in the model
     */
    int rowCount(const QModelIndex & = QModelIndex()) const;

    /**
     * @brief gets the data at a index for a specific role
     *
     * @param index index of the data to get
     * @param role data role for which data should be returned
     *
     * @return the requested data
     */
    virtual QVariant data(const QModelIndex &index, int role) const;

    /**
     * @brief sets the datalogger for which the model is created
     *
     * @param dataLogger the datalogger for which the model is created
     */
    void setDataLogger(DataLogger* dataLogger);
private:
    DataLogger* dataLogger; /**< parent datalogger */
private slots:
    /**
     * @brief slot to notify about a data change in the model
     */
    void dataChangedSlot();
};

#endif // DATALOGGEROTHERMODEL_H
