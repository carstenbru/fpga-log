/**
 * @file developertools.cpp
 * @brief DeveloperTools GUI class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include "developertools.h"
#include "ui_developertools.h"

#include "automaticcoreassigner.h"

DeveloperTools::DeveloperTools(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeveloperTools)
{
    ui->setupUi(this);

    connect(ui->moduleWeightsButton, SIGNAL(clicked(bool)), this, SLOT(calculateModuleWeights()));
}

DeveloperTools::~DeveloperTools()
{
    delete ui;
}

void DeveloperTools::calculateModuleWeights() {
    AutomaticCoreAssigner* aca = new AutomaticCoreAssigner();
    aca->calculateModuleWeights();
}
