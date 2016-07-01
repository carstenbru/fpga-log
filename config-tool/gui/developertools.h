/**
 * @file developertools.h
 * @brief DeveloperTools GUI class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef DEVELOPERTOOLS_H
#define DEVELOPERTOOLS_H

#include <QDialog>

namespace Ui {
class DeveloperTools;
}

/**
 * @brief developer tools dialog
 */
class DeveloperTools : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief constructor
     *
     * @param parent parent widget
     */
    explicit DeveloperTools(QWidget *parent = 0);
    ~DeveloperTools();

private:
    Ui::DeveloperTools *ui; /**< UI */

public slots:
    /**
     * @brief slot for triggering calculation of module weights
     */
    void calculateModuleWeights();
};

#endif // DEVELOPERTOOLS_H
