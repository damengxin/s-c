#ifndef QMEMOJI_H
#define QMEMOJI_H

#include <QWidget>
#include <QTableWidgetItem>

namespace Ui {
class QMEmoji;
}

class QMEmoji : public QWidget
{
    Q_OBJECT

public:
    explicit QMEmoji(QWidget *parent = nullptr);
    ~QMEmoji();
    void setTableVisible();

signals:
    void emoji(QString s);
    void visible();

private slots:
    void on_tableWidget_itemClicked(QTableWidgetItem *item);

private:
    Ui::QMEmoji *ui;
};

#endif // QMEMOJI_H
