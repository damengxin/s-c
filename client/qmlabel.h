#ifndef QMLABEL_H
#define QMLABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QWidget>

class QMLabel : public QLabel
{
    Q_OBJECT
private:
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

public:
    QMLabel(QWidget *parent=nullptr);

signals:
    void clicket();
};

#endif // QMLABEL_H
