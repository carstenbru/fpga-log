#ifndef MOVEABLEBUTTON_H
#define MOVEABLEBUTTON_H

#include <QPushButton>
#include <QMouseEvent>

class MoveableButton : public QPushButton
{
    Q_OBJECT
public:
    explicit MoveableButton(QWidget *parent = 0);
private:
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);

    QPoint clickPos;
signals:
    void moved();
public slots:

};

#endif // MOVEABLEBUTTON_H
