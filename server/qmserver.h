#ifndef QMSERVER_H
#define QMSERVER_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QDateTime>
#include <QMap>

#include "qmtcpserver.h"
#include "qmdata.h"
#include "qmemoji.h"

namespace Ui {
class QMServer;
}

class QMServer : public QMainWindow
{
    Q_OBJECT

public:
    //年月日，时分秒
    static QString time(){return QDateTime::currentDateTime()
                .toString("yyyy.MM.dd hh:mm:ss ");}
    //年月日,时分
    static QString times(){return QDateTime::currentDateTime()
                .toString("yyyy.MM.dd hh:mm ");}                
public:
    explicit QMServer(QWidget *parent = nullptr);
    ~QMServer();
    void ini();
    QString getUserIp(QString &userId);
    QByteArray getUserInfo(QString &userId);
    void userPrivate(QByteArray data);
public slots:
    void emojiMsg(QString s);
    void visible();
    void newConnect();
    void userLoginInfo(QString id);
    void userErrorInfo(QByteArray data);
    void dataError(QString error);
    void clientError(QString id,QByteArray error);
    void userDisconnect(bool b,QThread *thread);
    void frinedAdd(QString frinedid,QString userid);
    void upFrinedList(QString userId,QString frinedId);
    void on_start_triggered();
    void on_action_triggered();
    void udpRead();
    void udpSend(QByteArray info,QString ip,quint8 type);
    void readPublic();
    void sendPublic(quint8 type,QByteArray info);
    void getPublicUser();                          //27
    void userAddPublic(QByteArray info);           //41
    void userGone(QByteArray info);                //42
    void msgPublic(QByteArray info);               //43

signals:
    void frined(QByteArray data);
    void upList(QByteArray data);
    void publiOpen(bool isOpen);
    void publiClear();
    void send_Tcp(quint8 ,QByteArray);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::QMServer *ui;
    QMData *m_data;
    QMap<TcpSocket*,QString> m_SocketS;

    QHostAddress m_HostAddress;          //主机地址
    QMTcpServer *m_server=nullptr;       //服务器
    quint16 m_ServerPort;

    QUdpSocket *m_udpSocket=nullptr;     //私聊
    quint16 m_udpPort;

    bool isOpen=false;                   //群聊是否开启
    QUdpSocket *m_publicSocket=nullptr;  //群聊
    QList<QString> listId;               //群聊用户id
    QList<QString> listName;             //群聊用户名
    QHostAddress m_publicAddress;
    quint16 m_publicPort;
    QMEmoji *m_emoji=nullptr;
};

#endif // QMSERVER_H
