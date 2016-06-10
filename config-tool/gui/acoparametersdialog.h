#ifndef ACOPARAMETERSDIALOG_H
#define ACOPARAMETERSDIALOG_H

#include <QDialog>
#include "automaticcoreassigner.h"

namespace Ui {
class AcoParametersDialog;
}

class AcoParametersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AcoParametersDialog(AutomaticCoreAssigner* aco, QWidget *parent = 0);
    ~AcoParametersDialog();

private:
    Ui::AcoParametersDialog *ui;

    AutomaticCoreAssigner* aco;

private slots:
    void storeValues(int resultCode);
};

#endif // ACOPARAMETERSDIALOG_H
