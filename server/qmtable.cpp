#include "qmtable.h"

void QMTable::focusOutEvent(QFocusEvent *event)
{
    setVisible(false);
    emit visible();
}

QMTable::QMTable(QWidget *parent):
        QTableWidget(parent)
{

}
