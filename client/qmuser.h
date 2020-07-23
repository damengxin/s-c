#ifndef QMUSER_H
#define QMUSER_H

#include <QMainWindow>
#include <QWidget>
#include <QListWidget>
#include <QByteArray>

#include "userinfo.h"
#include "userchat.h"
#include "findfrined.h"
#include "uppass.h"
#include "userpublic.h"

namespace Ui {
class QMUser;
}

class QMUser : public QMainWindow
{
    Q_OBJECT
protected:
    void closeEvent(QCloseEvent *event);

public:
    explicit QMUser(QWidget *parent = nullptr);
    ~QMUser();
    void setIpPort(QString ip,quint16 port);
    void setUser(QByteArray &data);
    void userUnread(QByteArray data);          //1
    void systemUnread(QByteArray data);        //3
    void upList(QByteArray data);              //5
    void upUserInfo(QByteArray data);          //23
    void upPassword(QByteArray data);          //24
    void findUser(QByteArray data);            //25
    void userFrined(QByteArray data);          //26

public slots:
    void udpInfo(quint8 type,QString msg);
    void udp_readInfo(QByteArray info);
    void frined(quint8 type,QString id);
    void user_data(QByteArray data);
    void msgInfo(QString msg);
    void pass(QString pass);

signals:
    void up(QString);
    void renewLogin();
    void userError(QString error);
    void userErrorInfo(QByteArray info);
    void dis();
    void send_tcp(quint8 type,QByteArray info);
    void send_udp(quint8 type,QByteArray info);
    void isOpen(QByteArray info);                //27


private slots:
    void on_listWidget_customContextMenuRequested(const QPoint &pos);
    void on_listWidget_2_itemDoubleClicked(QListWidgetItem *item);
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_pushButton_4_clicked();
    void on_pushButton_3_clicked();

private:
    Ui::QMUser *ui;

    QString m_ip;
    quint16 m_port;

    QString m_id;
    QMap<QString,userChat*> m_chats;//私聊界面
    userPanel *m_panel=nullptr;     //用户详情
    findFrined *m_find=nullptr;     //添加好友
    findFrined *m_info=nullptr;     //系统消息
    upPass  *m_pass=nullptr;        //修改密码
    userPublic *m_public=nullptr;

};

#endif // QMUSER_H
