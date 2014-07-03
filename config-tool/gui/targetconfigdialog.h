#ifndef TARGETCONFIGDIALOG_H
#define TARGETCONFIGDIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QWidget>
#include "datalogger.h"

namespace Ui {
class TargetConfigDialog;
}

class TargetConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TargetConfigDialog(QWidget *parent, DataLogger* dataLogger);
    ~TargetConfigDialog();

private:
    void generateUi();

    Ui::TargetConfigDialog *ui;

    DataLogger* dataLogger;

    QWidget* widget;
    QWidget* clockPinWidget;
    QWidget* clockFreqWidget;
private slots:
    void targetChanged(QString newTarget);
    void pinChanged();
    void storeParams();
};

#endif // TARGETCONFIGDIALOG_H
