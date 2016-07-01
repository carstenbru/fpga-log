/**
 * @file pinbox.h
 * @brief PinBox class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef PINBOX_H
#define PINBOX_H

#include <QComboBox>
#include"datalogger.h"

/**
 * @brief two combo boxes for selection of a pin (group and pin) on the target platform
 */
class PinBox : public QComboBox
{
    Q_OBJECT

public:
    /**
     * @brief constructor
     *
     * @param dataLogger datalogger for which a pin should be selected
     * @param selectedPin currently selected pin
     * @param parent parent widget
     */
    explicit PinBox(DataLogger* dataLogger, std::string selectedPin,QWidget *parent = 0);
signals:
    /**
     * @brief emitted when the selected pin changed
     */
    void pinChanged();
public slots:
    /**
     * @brief slot to trigger an update of possible pins to select (e.g. when a pin in another box was selected or the group changed)
     *
     * @param pinGroup new pin group the pin should be part of
     */
    void setPinItems(const QString& pinGroup);

    /**
     * @brief slot to trigger an update of possible pins to select (e.g. when a pin in another box was selected or the group changed)
     */
    void setPinItems();
private slots:
    /**
     * @brief slot to notify about an index change
     */
    void indexChanged();
private:
    /**
     * @brief checks if a pin is already assigned
     *
     * @param pinAssignments map of pin assignments (key: pin parameter, value: assignment, see DataLogger::getPinAssignments())
     * @param name name of the pin to check
     * @return true if the pin is already assigned
     */
    bool pinAssigned(std::map<CParameter*, std::string[4]> &pinAssignments, std::string name);

    DataLogger* dataLogger; /**< datalogger to work on */
    std::string selectedPin; /**< pin selected in the widget */
    std::string pinGroup; /**< selected pin group */
    bool disablePinChange; /**< flag to indicate if the pin change signal should be emitted (e.g. to diable that when creating the pinbox items) */
};

#endif // PINBOX_H
