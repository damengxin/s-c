#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QTcpSocket>
#include <QByteArray>
#include <QTimer>

#include "qmdata.h"

////////////////////////////////////////////////////
/// 每个TcpSocket(用户)都是一个线程,相互之间通信采用信号-槽。
/// 比如：添加好友功能
/// 由本socket的非好友发出addFrined。
/// 本socket的frined向服务器发出frinedAdd信号，
/// 由服务器寻找目标socket,发出frinedAdd信号。
/// 由目标socket使用frined槽向用户发出信息。
/// 用户同意userFrined后更新用户列表upList，
/// 同时发送信号upFrinedList由服务器转发更新好友的好友列表。
///
////////////////////////////////////////////////////
/// getUserInfo通过id获取用户的信息，不包含密码，密保等
/// getUserFrined用户的好友信息
/// userIdData用户的登录的信息
/// userUnreadInfo获取用户的未读信息并分类
/// privateInfo返回用户的离线信息
/// unreadFrined用户不在线时的加好友信息
/// addFrined用户添加好友
/// userFrined用户同意别人添加好友请求
/// userLoginCheck用户登录验证
/// userNew申请账号
/// userInfoUp修改用户信息
///
////////////////////////////////////////////////////
/// SLOT
/// upList更新用户的好友列表
/// frined有非好友请求添加好友，参数1请求加参数(用户)2为好友
/// setSocketHandle设置socket的描述符
/// read_Data读取信息
/// send_Data发送信息
/// disconnect()连接断开
///
////////////////////////////////////////////////////
/// SIGNAL
/// error服务器操作错误
/// userErrorInfo用户发送的无法识别信息
/// userIoginInfo用户的登录信息
/// frinedAdd通知用户有非好友要加好友，参数(用户)1要加参数2为好友
/// frinedList更新用户的好友列表，
/// frinedList参数1，用户id，参数1用户好友信息，使用List保存每个好友的信息
///
////////////////////////////////////////////////////

class TcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    TcpSocket(QObject *parent=nullptr);
    ~TcpSocket();
    void useData(QByteArray &data);
    /////////////服务器处理用户信息///////////////////////
    QByteArray getUserInfo(QString userId);
    QByteArray getUserFrined(QString &id);
    void userUnreadInfo(QString &id);
    void unreadPrivateInfo(QList<QByteArray> list);//1
    void unreadAddFrined(QList<QByteArray> list);  //~4

/////////////客户端用户信息处理///////////////////////////////
    void addFrined(QByteArray data);               //2
    void userFrined(QByteArray data);              //4
    void clientError(QByteArray data);             //6
    void heart(QByteArray data);                   //7
    void userLoginCheck(QByteArray data);          //21
    void userNew(QByteArray data);                 //22
    void userInfoUp(QByteArray data);              //23
    void userPassUp(QByteArray data);              //24
    void fendUser(QByteArray data);                //25
    void userFrinedFend(QByteArray data);          //26
    void forget(QByteArray data);                  //28

public slots:
    void frined(QByteArray data);                  //3
    void upList(QByteArray frinedInfo);            //5
    void isManySpeakOpen();                        //27
    void setSocketHandle(qintptr handle);
    void read_Data();
    void send_Data(quint8 type,QByteArray info);
    void userDisConnect();
    void timerHeart();

signals:
///////////////////UI操作///////////////////////////////
///控制界面显示信息
    void dis(bool b,QThread*);
    void dataError(QString error);
    void client(QString id,QByteArray error);
    void userErrorInfo(QByteArray info);
    void userLoginInfo(QString id);
    void publicUser();

//////////////////////socket之间通信////////////////////
///通过服务器中转
    void frinedAdd(QString userId,QString frinedId);
    void upFrinedList(QString userId,QString frinedid);

///////////////////////////////////////////////////////
private:
    QMData *m_data=nullptr;
    QByteArray buffer;      //存放socket信息
    QByteArray buffer_2;    //组包
    bool isLogin;           //该连接用户是否登录
    quint8 jump;
    QTimer *m_time;
    QString m_dataConnectName;
};

#endif // TCPSOCKET_H
