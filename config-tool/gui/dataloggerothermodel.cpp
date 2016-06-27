/**
 * @file dataloggerothermodel.cpp
 * @brief DataLoggerOtherModel class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include "dataloggerothermodel.h"
#include "cobject.h"

DataLoggerOtherModel::DataLoggerOtherModel(QObject *parent) :
    QAbstractListModel(parent),
    dataLogger(NULL)
{

}

void DataLoggerOtherModel::setDataLogger(DataLogger* dataLogger) {
    this->dataLogger = dataLogger;
    connect(dataLogger, SIGNAL(otherModulesChanged()), this, SLOT(dataChangedSlot()));

    emit dataChanged(QModelIndex(), QModelIndex());
}

int DataLoggerOtherModel::rowCount(const QModelIndex &) const {
    if (dataLogger != NULL) {
        return dataLogger->getOtherObjects().size();
    } else
        return 0;
}

QVariant DataLoggerOtherModel::data(const QModelIndex &index, int role) const {
     if ((role == Qt::DisplayRole) || (role == Qt::UserRole + 1)) {
         if (dataLogger != NULL) {
             if (index.row() < (int)dataLogger->getOtherObjects().size()) {
                 return dataLogger->getOtherObjects().at(index.row())->getName().c_str();
             }
         }
     }
     return QVariant();
}

void DataLoggerOtherModel::dataChangedSlot() {
    emit dataChanged(QModelIndex(), QModelIndex());
}
