/**
 * @file targetconfigdialog.h
 * @brief TargetConfigDialog GUI class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

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

/**
 * @brief dialog for configuration target parameters
 */
class TargetConfigDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief constructor
     *
     * @param parent parent widget
     * @param dataLogger datalogger for which the target should be configured
     */
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
