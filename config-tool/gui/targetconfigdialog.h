#ifndef TARGETCONFIGDIALOG_H
#define TARGETCONFIGDIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QWidget>
#include "datalogger.h"
#include <map>
#include <set>

namespace Ui {
class TargetConfigDialog;
}

class TargetConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TargetConfigDialog(QWidget *parent, DataLogger* dataLogger);
    ~TargetConfigDialog();

private:
    void generateUi();

    Ui::TargetConfigDialog *ui;

    DataLogger* dataLogger;

    QWidget* widget;
    QWidget* clockPinWidget;
    QWidget* clockFreqWidget;
    QWidget* expertModeWidget;

    QComboBox* systemClkSelect;

    std::set<int> frequencies;
    std::map<int, std::pair<int,int>> freqCoefficients;
private slots:
    void targetChanged(QString newTarget);
    void pinChanged();
    void storeParams();

    void freqChanged(int clockFreqValue);
};

#endif // TARGETCONFIGDIALOG_H
