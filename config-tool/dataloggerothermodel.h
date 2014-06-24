#ifndef DATALOGGEROTHERMODEL_H
#define DATALOGGEROTHERMODEL_H

#include <QAbstractListModel>
#include "datalogger.h"

class DataLoggerOtherModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit DataLoggerOtherModel(DataLogger* dataLogger, QObject *parent = 0);

    int rowCount(const QModelIndex & = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
private:
    DataLogger* dataLogger;
private slots:
    void dataChangedSlot();
};

#endif // DATALOGGEROTHERMODEL_H
