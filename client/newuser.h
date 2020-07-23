#ifndef NEWUSER_H
#define NEWUSER_H

#include <QWidget>

#include "qmavatar.h"

namespace Ui {
class newUser;
}

class newUser : public QWidget
{
    Q_OBJECT

public:
    explicit newUser(QWidget *parent = nullptr);
    ~newUser();
    QString encrypt(QString &str);
    void setId(QString id);

private slots:
    void setPhoto(QByteArray photo);
    void on_pushButton_clicked();

    void on_pass_2_editingFinished();

    void on_photo_clicket();

signals:
    void new_User(quint8 type,QByteArray info);

private:
    Ui::newUser *ui;
    QMavatar *m_avatar=nullptr;
    QByteArray m_photo;
};

#endif // NEWUSER_H
