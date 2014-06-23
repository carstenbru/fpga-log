#include "moveablebutton.h"

MoveableButton::MoveableButton(QWidget *parent) :
    QPushButton(parent)
{
}

void MoveableButton::mouseMoveEvent(QMouseEvent *e) {
    if (e->buttons() & Qt::LeftButton) {
        move(parentWidget()->mapFromGlobal(e->globalPos()) - clickPos);
        emit moved();
   }
}

void MoveableButton::mousePressEvent(QMouseEvent *e) {
   if (e->buttons() & Qt::LeftButton) {
        clickPos = e->pos();
   }
   QPushButton::mousePressEvent(e);
}
