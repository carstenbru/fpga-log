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
    explicit PinOverview(QWidget *parent, DataLogger *dataLogger, std::string dataLoggerPath);
    ~PinOverview();
private slots:
    void exportOverview();
private:
    void readPinAssignments();
    void writeHTMLtable(std::ofstream &file);

    Ui::PinOverview *ui;
    DataLogger* dataLogger;
    std::string dataLoggerPath;

    std::map<std::string, QStandardItem*[4]> items;
};

#endif // PINOVERVIEW_H
