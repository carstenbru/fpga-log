#ifndef NEWMETHODDIALOG_H
#define NEWMETHODDIALOG_H

#include <QDialog>
#include "cmethod.h"

namespace Ui {
class NewMethodDialog;
}

class NewMethodDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewMethodDialog(std::list<CMethod*> methods, QWidget *parent = 0);
    ~NewMethodDialog();

    std::string getSelectedMethod();

private:
    Ui::NewMethodDialog *ui;
};

#endif // NEWMETHODDIALOG_H
