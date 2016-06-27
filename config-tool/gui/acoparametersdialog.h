/**
 * @file acoparametersdialog.h
 * @brief AcoParametersDialog GUI class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef ACOPARAMETERSDIALOG_H
#define ACOPARAMETERSDIALOG_H

#include <QDialog>
#include "automaticcoreassigner.h"

namespace Ui {
class AcoParametersDialog;
}

/**
 * @brief GUI dialog class for setting parameters of the AutomaticCoreAssigner
 */
class AcoParametersDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief constructor
     *
     * @param aco AutomaticCoreAssigner for which the parameters should be configured
     * @param parent parent widget object
     */
    explicit AcoParametersDialog(AutomaticCoreAssigner* aco, QWidget *parent = 0);
    ~AcoParametersDialog();

private:
    Ui::AcoParametersDialog *ui;

    AutomaticCoreAssigner* aco;

private slots:
    void storeValues(int resultCode);
};

#endif // ACOPARAMETERSDIALOG_H
