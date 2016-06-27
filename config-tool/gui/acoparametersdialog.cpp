/**
 * @file acoparametersdialog.cpp
 * @brief AcoParametersDialog GUI class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include "acoparametersdialog.h"
#include "ui_acoparametersdialog.h"

AcoParametersDialog::AcoParametersDialog(AutomaticCoreAssigner* aco, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AcoParametersDialog),
    aco(aco)
{
    ui->setupUi(this);

    ui->minCoresBox->setValue(aco->getMinCores());
    ui->maxCoresBox->setValue(aco->getMaxCores());
    ui->maxWeightPerCoreBox->setValue(aco->getMaxWeightPerCore());
    ui->dataStreamWeightBox->setValue(aco->getDataStreamWeight());
    ui->controlStreamWeightBox->setValue(aco->getControlStreamWeight());

    connect(this, SIGNAL(finished(int)), this, SLOT(storeValues(int)));
}

AcoParametersDialog::~AcoParametersDialog()
{
    delete ui;
}

void AcoParametersDialog::storeValues(int resultCode) {
    if (resultCode == QDialog::Accepted) {
        aco->setMinCores(ui->minCoresBox->value());
        aco->setMaxCores(ui->maxCoresBox->value());
        aco->setMaxWeightPerCore(ui->maxWeightPerCoreBox->value());
        aco->setDataStreamWeight(ui->dataStreamWeightBox->value());
        aco->setControlStreamWeight(ui->controlStreamWeightBox->value());
    }
}
