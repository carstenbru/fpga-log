/**
 * @file moveablebutton.h
 * @brief MoveableButton class
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef MOVEABLEBUTTON_H
#define MOVEABLEBUTTON_H

#include <QPushButton>
#include <QMouseEvent>

/**
 * @brief class extending a QPushButton to be able to be moved by the user (by holding the left mouse button)
 */
class MoveableButton : public QPushButton
{
    Q_OBJECT
public:
    /**
     * @brief constructor
     *
     * @param parent parent widget
     */
    explicit MoveableButton(QWidget *parent = 0);
    virtual ~MoveableButton() {}
private:
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

    QPoint clickPos; /**< click position */
    bool isClick; /**< flag indicating if the current action is a (left) click */
    bool isRightClick; /**< flag indicating if the current action is a right click */
signals:
    /**
     * @brief triggered whenever the button was moved
     */
    void moved();
    /**
     * @brief triggered whenever the button was moved
     *
     * @param oldPos old button postion
     * @param newPos new updated position
     */
    void moved(QPoint oldPos, QPoint newPos);
    /**
     * @brief emitted when the button was clicked
     */
    void clicked();
    /**
     * @brief emitted when the button was right clicked
     */
    void rightClicked(QPoint pos);
};

#endif // MOVEABLEBUTTON_H
