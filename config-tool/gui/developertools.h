#ifndef DEVELOPERTOOLS_H
#define DEVELOPERTOOLS_H

#include <QDialog>

namespace Ui {
class DeveloperTools;
}

class DeveloperTools : public QDialog
{
    Q_OBJECT

public:
    explicit DeveloperTools(QWidget *parent = 0);
    ~DeveloperTools();

private:
    Ui::DeveloperTools *ui;

public slots:
    void calculateModuleWeights();
};

#endif // DEVELOPERTOOLS_H
