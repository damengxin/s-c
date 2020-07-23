#ifndef QMAVATAR_H
#define QMAVATAR_H

#include <QDialog>
#include <QListWidgetItem>
#include <QFocusEvent>

namespace Ui {
class QMavatar;
}

class QMavatar : public QDialog
{
    Q_OBJECT

public:
    explicit QMavatar(QWidget *parent = nullptr);
    ~QMavatar();
    QByteArray getIconInfo(){return photo;}
private slots:
    void on_listWidget_itemClicked(QListWidgetItem *item);

signals:
    void iconinfo(QByteArray data);

private:
    Ui::QMavatar *ui;
    //保存默认的图片信息
    QList<QByteArray> m_li;
    QByteArray photo;
};

#endif // QMAVATAR_H
