#ifndef PINOVERVIEW_H
#define PINOVERVIEW_H

#include <QDialog>
#include <QStandardItemModel>
#include "datalogger.h"

namespace Ui {
class PinOverview;
}

class PinOverview : public QDialog
{
    Q_OBJECT

public:
    explicit PinOverview(QWidget *parent, DataLogger *dataLogger);
    ~PinOverview();

private:
    void readPinAssignments();

    Ui::PinOverview *ui;
    DataLogger* dataLogger;

    std::map<std::string, QStandardItem*[4]> items;
};

#endif // PINOVERVIEW_H
