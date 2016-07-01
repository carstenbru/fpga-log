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
    /**
     * @brief generates and setups the UI of the dialog (i.e. adding all elements)
     */
    void generateUi();

    Ui::TargetConfigDialog *ui; /**< UI */

    DataLogger* dataLogger; /**< datalogger to work on */

    QWidget* widget; /**< main dialog widget */
    QWidget* clockPinWidget; /**< clock pin configuration widget */
    QWidget* clockFreqWidget; /**< clock pin input frequency selection widget */
    QWidget* expertModeWidget; /**< expert mode select widget */

    QComboBox* systemClkSelect; /**< system frequency selection widget */

    std::set<int> frequencies; /**< set of achieveable frequencies with the DCM */
    std::map<int, std::pair<int,int>> freqCoefficients; /**< coefficients to achieve the frequencies int the set "frequencies" (key: frqeuncy, value: divider, multiplier */
private slots:
    /**
     * @brief slot to notify about a target platform change
     * @param newTarget
     */
    void targetChanged(QString newTarget);
    /**
     * @brief slot to notify about a change of the clock pin
     */
    void pinChanged();
    /**
     * @brief slot to trigger storage of all parameters
     */
    void storeParams();

    /**
     * @brief slot to notify about a change in the input clock frequency
     *
     * @param clockFreqValue new input clock frequency in Hz
     */
    void freqChanged(int clockFreqValue);
};

#endif // TARGETCONFIGDIALOG_H
