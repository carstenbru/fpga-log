/**
 * @file pinoverview.h
 * @brief PinOverview GUI class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef PINOVERVIEW_H
#define PINOVERVIEW_H

#include <QDialog>
#include <QStandardItemModel>
#include "datalogger.h"

namespace Ui {
class PinOverview;
}

/**
 * @brief dialog showing an overview of the systems pins and enabling assignment of pins
 */
class PinOverview : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief constructor
     *
     * @param parent parent widget
     * @param dataLogger datalogger for which the pins should be shown
     * @param dataLoggerPath path of the datalogger (for HTML export)
     */
    explicit PinOverview(QWidget *parent, DataLogger *dataLogger, std::string dataLoggerPath);
    ~PinOverview();
private slots:
    /**
     * @brief slot to trigger HTML export
     */
    void exportOverview();
    /**
     * @brief frees a pin (removes assignment)
     */
    void freePin();
    /**
     * @brief slot to assign a pin with the function selected in the pending box
     */
    void assignPin();
private:
    /**
     * @brief reads pin assignments from the datalogger and add them to the internal data models
     */
    void readPinAssignments();
    /**
     * @brief writes a HTML table with the current pin assignments
     *
     * @param file destination file
     */
    void writeHTMLtable(std::ofstream &file);
    /**
     * @brief adds a pending assignment to the model and view
     *
     * @param definition defintion of the assignement (module, group, function, direction)
     */
    void addPendingAssignment(std::string definition[4]);

    Ui::PinOverview *ui; /**< UI */
    DataLogger* dataLogger; /**< datalogger to visualize */
    std::string dataLoggerPath; /**< path of the datalogger (for HTML export) */

    QStandardItemModel* pendingAssignmentsModel; /**< data model for pending pin assignments */

    std::map<std::string, QStandardItem*[4]> items; /**<  (key: pin, value: view items (pin, module, group, function) */
    std::map<std::string, CParameter*> pinParameters; /**< parameters for the pins (key: pin name, value: parameter */

    std::map<std::string, QStandardItem*> pendingAssignmentGroups; /**< pin groups with pending assignments (key: module, value: item of the group) */
};

#endif // PINOVERVIEW_H
