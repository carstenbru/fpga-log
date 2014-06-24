#include "dataloggerothermodel.h"
#include "cobject.h"

DataLoggerOtherModel::DataLoggerOtherModel(DataLogger *dataLogger, QObject *parent) :
    QAbstractListModel(parent),
    dataLogger(dataLogger)
{
    connect(dataLogger, SIGNAL(otherModulesChanged()), this, SLOT(dataChangedSlot()));
}

int DataLoggerOtherModel::rowCount(const QModelIndex &) const {
    return dataLogger->getOtherObjects().size();
}

QVariant DataLoggerOtherModel::data(const QModelIndex &index, int role) const {
     if ((role == Qt::DisplayRole) || (role == Qt::UserRole + 1)) {
         return dataLogger->getOtherObjects().at(index.row())->getName().c_str();
     }
     return QVariant();
}

void DataLoggerOtherModel::dataChangedSlot() {
    emit dataChanged(QModelIndex(), QModelIndex());
}
