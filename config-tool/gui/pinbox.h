#ifndef PINBOX_H
#define PINBOX_H

#include <QComboBox>
#include"datalogger.h"

class PinBox : public QComboBox
{
    Q_OBJECT

public:
    explicit PinBox(DataLogger* dataLogger, std::string selectedPin,QWidget *parent = 0);
signals:
    void pinChanged();
public slots:
    void setPinItems(const QString& pinGroup);
private slots:
    void indexChanged();
private:
    bool pinAssigned(std::list<std::string[4]>& pinAssignments, std::string name);

    DataLogger* dataLogger;
    std::string selectedPin;
};

#endif // PINBOX_H
