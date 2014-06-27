#ifndef PINBOX_H
#define PINBOX_H

#include <QComboBox>

class PinBox : public QComboBox
{
    Q_OBJECT

public:
    explicit PinBox(QWidget *parent = 0);

signals:

public slots:
    void setPinItems(const QString& pinGroup);
};

#endif // PINBOX_H
