/**
 * @file moveablebutton.cpp
 * @brief MoveableButton class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include "moveablebutton.h"

MoveableButton::MoveableButton(QWidget *parent) :
    QPushButton(parent),
    isClick(false),
    isRightClick(false)
{
}

void MoveableButton::mouseMoveEvent(QMouseEvent *e) {
    if (e->buttons() & Qt::LeftButton) {
        isClick = false;
        QPoint oldPos = pos();
        move(parentWidget()->mapFromGlobal(e->globalPos()) - clickPos);
        emit moved();
        emit moved(oldPos, pos());
   }
}

void MoveableButton::mousePressEvent(QMouseEvent *e) {
   if (e->buttons() & Qt::LeftButton) {
        clickPos = e->pos();
        isClick = true;
   }
   if (e->buttons() & Qt::RightButton) {
        isRightClick = true;
   }
   QPushButton::mousePressEvent(e);
}

void MoveableButton::mouseReleaseEvent(QMouseEvent *e) {
    if ((e->buttons() == Qt::NoButton) && (isClick || isRightClick)) {
        if (isClick) {
            isClick = false;
            emit clicked();
        }
        if (isRightClick) {
            isRightClick = false;
            emit rightClicked(e->pos());
        }
    } else
        QPushButton::mouseReleaseEvent(e);
}
