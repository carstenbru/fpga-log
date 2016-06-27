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
    void exportOverview();
    void freePin();
    void assignPin();
private:
    void readPinAssignments();
    void writeHTMLtable(std::ofstream &file);
    void addPendingAssignment(std::string definition[3]);

    Ui::PinOverview *ui;
    DataLogger* dataLogger;
    std::string dataLoggerPath;

    QStandardItemModel* pendingAssignmentsModel;

    std::map<std::string, QStandardItem*[4]> items;
    std::map<std::string, CParameter*> pinParameters;

    std::map<std::string, QStandardItem*> pendingAssignmentGroups;
};

#endif // PINOVERVIEW_H
