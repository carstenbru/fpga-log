#include "moveablebutton.h"

MoveableButton::MoveableButton(QWidget *parent) :
    QPushButton(parent),
    isClick(false)
{
}

void MoveableButton::mouseMoveEvent(QMouseEvent *e) {
    if (e->buttons() & Qt::LeftButton) {
        isClick = false;
        move(parentWidget()->mapFromGlobal(e->globalPos()) - clickPos);
        emit moved();
   }
}

void MoveableButton::mousePressEvent(QMouseEvent *e) {
   if (e->buttons() & Qt::LeftButton) {
        clickPos = e->pos();
        isClick = true;
   }
   QPushButton::mousePressEvent(e);
}

void MoveableButton::mouseReleaseEvent(QMouseEvent *e) {
    if ((e->buttons() == Qt::NoButton) && (isClick)) {
        isClick = false;
        emit clicked();
    }
    QPushButton::mouseReleaseEvent(e);
}
