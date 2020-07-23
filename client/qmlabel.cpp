#include "qmlabel.h"

void QMLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    //Q_UNUSED(event)
    if(event->button()==Qt::LeftButton)
        emit clicket();
}

QMLabel::QMLabel(QWidget *parent):
    QLabel(parent)
{

}
