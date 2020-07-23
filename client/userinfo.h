#ifndef USERINFO_H
#define USERINFO_H

#include <QDialog>
#include "userpanel.h"

namespace Ui {
class userInfo;
}

class userInfo : public QDialog
{
    Q_OBJECT

public:
    explicit userInfo(QWidget *parent = nullptr);
    ~userInfo();
    void setUser(QByteArray &user);
    void setmodify(bool n);
    QByteArray getUser(){return m_panel->getUser();}
    QString& getUserId(){return m_panel->getUserId();}
    QString& getUserNmae(){return m_panel->getUserNmae();}
    bool& getUserSex(){return m_panel->getUserSex();}
    int& getUserAge(){return m_panel->getUserAge();}
    QString& getUserArea(){return m_panel->getUserArea();}
    QByteArray& getPhoto(){return m_panel->getUserPhoto();}
    QString& getMotto(){return m_panel->getUserMotto();}
    void addVisible(bool b);
    void addEnabled(bool b);
    void agreeVisible(bool b);


private slots:
    void on_photo_clicket();
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_4_clicked();

signals:
    void add(quint8 type,QString id);  //2
    void agree(quint8 type,QString id);//4
    void clear();

private:
    Ui::userInfo *ui;
    userPanel *m_panel=nullptr;
};

#endif // USERINFO_H
