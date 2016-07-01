/**
 * @file newmethoddialog.h
 * @brief NewMethodDialog GUI class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef NEWMETHODDIALOG_H
#define NEWMETHODDIALOG_H

#include <QDialog>
#include "cmethod.h"

namespace Ui {
class NewMethodDialog;
}

/**
 * @brief dialog for selecting a fpga-log method (advanced logger config)
 */
class NewMethodDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief constructor
     *
     * @param methods list of possible methods for the selection
     * @param parent parent widget
     */
    explicit NewMethodDialog(std::list<CMethod*> methods, QWidget *parent = 0);
    ~NewMethodDialog();

    /**
     * @brief gets the name of the mehtod selected by the user
     *
     * @return the name of the mehtod selected by the user
     */
    std::string getSelectedMethod();

private:
    Ui::NewMethodDialog *ui; /**< UI */
};

#endif // NEWMETHODDIALOG_H
