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
 * @brief combo box for selection of a pin on the target platform
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
    void indexChanged();
private:
    bool pinAssigned(std::map<CParameter*, std::string[4]> &pinAssignments, std::string name);

    DataLogger* dataLogger;
    std::string selectedPin;
    std::string pinGroup;
    bool disablePinChange;
};

#endif // PINBOX_H
