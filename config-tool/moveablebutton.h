#ifndef MOVEABLEBUTTON_H
#define MOVEABLEBUTTON_H

#include <QPushButton>
#include <QMouseEvent>

class MoveableButton : public QPushButton
{
    Q_OBJECT
public:
    explicit MoveableButton(QWidget *parent = 0);
    virtual ~MoveableButton() {}
private:
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

    QPoint clickPos;
    bool isClick;
signals:
    void moved();
    void clicked();
};

#endif // MOVEABLEBUTTON_H
