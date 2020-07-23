#ifndef USERPANEL_H
#define USERPANEL_H

#include <QWidget>
#include "qmavatar.h"

namespace Ui {
class userPanel;
}

class userPanel : public QWidget
{
    Q_OBJECT

public:
    explicit userPanel(QWidget *parent = nullptr);
    ~userPanel();
    void setUser(QByteArray &user);
    void setmodify(bool n);
    void setUpVisible(bool b);
    QByteArray& getUser(){return m_user;}
    QString& getUserId(){return m_id;}
    QString& getUserNmae(){return m_name;}
    bool& getUserSex(){return m_sex;}
    int& getUserAge(){return m_age;}
    QString& getUserArea(){return m_area;}
    QByteArray& getUserPhoto(){return m_photo;}
    QString& getUserMotto(){return m_motto;}
    void setUp(bool b);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_photo_clicket();

    void setPhoto(QByteArray photo);

signals:
    void userInfo(quint8 type,QByteArray info);
private:
    Ui::userPanel *ui;
    QMavatar *m_avatar=nullptr;
    QString m_id;
    QString m_name;
    bool m_sex;
    int m_age;
    QString m_area;
    QByteArray m_photo;
    QString m_motto;
    QByteArray m_user;
};

#endif // USERPANEL_H
