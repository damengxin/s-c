#ifndef QMTABLE_H
#define QMTABLE_H

#include <QObject>
#include <QTableWidget>

class QMTable :public QTableWidget
{
    Q_OBJECT
private:
        void focusOutEvent(QFocusEvent *event);
signals:
        void visible();
public:
    QMTable(QWidget *parent);
};

#endif // QMTABLE_H
