#include "tcpsocket.h"
#include <QDataStream>
#include <QHostAddress>
#include <QUuid>
#include "qmserver.h"

TcpSocket::TcpSocket(QObject *parent):
    QTcpSocket(parent)
{
    isLogin=false;
    QUuid uuid=QUuid::createUuid();
    m_dataConnectName=uuid.toString()
            .remove("{").remove("}");
    if(m_data==nullptr)
        m_data=new QMData("QSQLITE",m_dataConnectName,this);
    connect(m_data,&QMData::dataError,
            this,&TcpSocket::dataError);
    m_time=new QTimer(this);
    connect(m_time,&QTimer::timeout,
            this,&TcpSocket::timerHeart);
    jump=0;
    m_time->start(1000*10);
}

TcpSocket::~TcpSocket()
{
    qDebug()<<"socket delete";
}

void TcpSocket::setSocketHandle(qintptr handle)
{
    this->setSocketDescriptor(handle);
}

void TcpSocket::read_Data()
{//读取
    buffer.clear();
    buffer=this->readAll();
    quint8 index;
    qint32 size;
    buffer_2+=buffer;
    QDataStream in(&buffer_2,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    in>>index;
    qDebug()<<"读取信息"<<index;
    if(index==2||index==4||index==7||index==21||index==22||index==23
            ||index==24||index==25||index==26||index==27||index==28)
    {
        in>>size;
        if(size==buffer_2.size())
        {
            useData(buffer_2);
        }
        return;
    }
    else
    {
        qDebug()<<"userError";
        emit userErrorInfo(buffer_2);
        buffer_2.clear();
        return;
    }

}

void TcpSocket::send_Data(quint8 type,QByteArray data)
{//发送
    QByteArray info;
    int size=0;
    QDataStream out(&info,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out<<type<<size<<data;
    out.device()->seek(0);
    out<<type<<info.size();
    this->write(info);
    qDebug()<<"tcpSend"<<type<<info.size();
}

void TcpSocket::userDisConnect()
{
    isLogin=false;
    m_time->stop();
    m_data->remove(m_dataConnectName);
    delete m_data;
    delete m_time;
    QThread *thread=QThread::currentThread();
    emit dis(isLogin,thread);
    thread->quit();
}

void TcpSocket::timerHeart()
{
    jump++;
    if(jump==3)//连续3次没有接到心跳
    {
        emit disconnected();
    }
}

void TcpSocket::useData(QByteArray &data)
{//信息分类处理
    quint8 index;
    int size;
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QByteArray da,error;
    in>>index>>size>>da;
    error=data;
    data.clear();
    qDebug()<<"信息分类 "<<index;
    switch (index)
    {
    case 2:addFrined(da);return;
    case 4:userFrined(da);return;
    case 7:heart(da);return;
    case 21:userLoginCheck(da);return;
    case 22:userNew(da);return;
    case 23:userInfoUp(da);return;
    case 24:userPassUp(da);return;
    case 25:fendUser(da);return;
    case 26:userFrinedFend(da);return;
    case 27:isManySpeakOpen();return;
    case 28:forget(da);return;
    default:emit userErrorInfo(error);return;
    }
}

QByteArray TcpSocket::getUserInfo(QString userId)
{
    QByteArray data;
    QSqlRecord re=m_data->userSelect(userId);
    if(re.isEmpty())
        return data;
    QString id=re.value("user_id").toString();
    QString name=re.value("user_name").toString();
    bool sex=re.value("user_sex").toBool();
    int age=re.value("user_age").toInt();
    QString area=re.value("user_area").toString();
    QByteArray photo=re.value("user_photo").toByteArray();
    QString motto=re.value("user_motto").toString();
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out<<id<<name<<sex<<age<<area<<photo<<motto;
    return data;
}

QByteArray TcpSocket::getUserFrined(QString &id)
{
    QList<QSqlRecord> list=m_data->buddySelect(id);
    int count=list.count();
    QSqlRecord re;
    QList<QByteArray> li;
    for(int i=0;i<count;i++)
    {
        re=list.at(i);
        QString id=re.value("user_id").toString();
        QString name=re.value("user_name").toString();
        bool sex=re.value("user_sex").toBool();
        int age=re.value("user_age").toInt();
        QString area=re.value("user_area").toString();
        QByteArray photo=re.value("user_photo").toByteArray();
        QString motto=re.value("user_motto").toString();
        QByteArray info;
        QDataStream cout(&info,QIODevice::WriteOnly);
        cout.setVersion(QDataStream::Qt_5_13);
        cout<<id<<name<<sex<<age<<area<<photo<<motto;
        li.append(info);
    }
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    count=li.count();
    out<<count<<li;
    return data;
}

void TcpSocket::userUnreadInfo(QString &id)
{//获取未读信息
    QList<QSqlRecord> list=m_data->getInfo(id);
    QList<QByteArray> unreadInfo;     //保存用户之间的未读信息
    QList<QByteArray> frinedInfo;     //加好友信息
///可以自定义信息类型，，要求大于60,客户端需要同步修改
    int count=list.count();
    qDebug()<<count;
    if(count>0)
    {
        quint8 type;
        QSqlRecord re;
        QByteArray data;
        for(int i=0;i<count;i++)
        {
            re=list.at(i);
            type=re.value(2).toInt();
            data=re.value(3).toByteArray();
            qDebug()<<"unread"<<count<<type;
            switch (type)
            {
            case 1:unreadInfo.append(data);break;
            case 3:frinedInfo.append(data);break;
            }
        }
        if(!unreadInfo.isEmpty())
        {qDebug()<<"unread"<<"info";
            unreadPrivateInfo(unreadInfo);
        }
        if(!frinedInfo.isEmpty())
        {qDebug()<<"unread"<<"frined";
            unreadAddFrined(frinedInfo);
        }
        //m_data->clearInfo(id);
    }
}

void TcpSocket::unreadPrivateInfo(QList<QByteArray> list)
{//1
    int count=list.count();
    QByteArray info;
    QDataStream out(&info,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out<<count<<list;
    qDebug()<<count;
    quint8 type=1;
    qDebug()<<tr("type%1").arg(type);
    send_Data(type,info);
}

void TcpSocket::addFrined(QByteArray data)
{//2
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QString userid,frinedid;
    in>>userid>>frinedid;
    emit frinedAdd(frinedid,userid);
}

void TcpSocket::frined(QByteArray data)
{//3
    QList<QByteArray> list;
    list.append(data);
    quint8 type=3;
    int count=list.count();
    QByteArray info;
    QDataStream out(&info,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out<<count<<list;
    qDebug()<<tr("type3");
    send_Data(type,info);
}

void TcpSocket::unreadAddFrined(QList<QByteArray> list)
{//~3不在线
    quint8 type=3;
    int count=list.count();
    QByteArray info;
    QDataStream out(&info,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out<<count<<list;
    qDebug()<<count;
    qDebug()<<tr("type~3");
    send_Data(type,info);
}

void TcpSocket::userFrined(QByteArray data)
{//4
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QString userid,frinedid;
    in>>userid>>frinedid;
    QByteArray frined=getUserInfo(frinedid);
    m_data->buddyIncrease(userid,frinedid);
    upList(frined);//刷新用户列表
    emit upFrinedList(frinedid,userid);//刷新好友列表
}

void TcpSocket::upList(QByteArray frinedInfo)
{//5
    quint8 type=5;
    qDebug()<<tr("type%1").arg(type);
    send_Data(type,frinedInfo);
}

void TcpSocket::clientError(QByteArray data)
{//6
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QString userid;
    QByteArray error;
    in>>userid>>error;
    emit client(userid,error);
}

void TcpSocket::heart(QByteArray data)
{//7
    jump=0;
    m_time->start();
}

void TcpSocket::userLoginCheck(QByteArray data)
{//21
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QString userid,pass;
    in>>userid>>pass;
    QSqlRecord re=m_data->userSelect(userid);
    QString password=re.value("user_password").toString();
    QByteArray info;
    QDataStream out(&info,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    quint8 type=21;
    bool is=false;
    if(pass==password)
    {
        is=true;
        out<<is<<getUserInfo(userid);
        send_Data(type,info);
    }
    else
    {
        is=false;
        out<<is;
        send_Data(type,info);
    }
}

void TcpSocket::userNew(QByteArray data)
{//22
    QString id=m_data->userIncrease(data);
    if(!id.isEmpty())
    {
        QByteArray info;
        QDataStream out(&info,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_13);
        quint8 type=22;
        out<<id;
        send_Data(type,info);
    }
}

void TcpSocket::userInfoUp(QByteArray data)
{//23
    bool is=m_data->userModify(data);
    QByteArray info;
    QDataStream out(&info,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    quint8 type=23;
    out<<is;
    qDebug()<<tr("type%1").arg(type);
    send_Data(type,info);
}

void TcpSocket::userPassUp(QByteArray data)
{//24
    bool is=m_data->userPassModify(data);
    QByteArray info;
    QDataStream out(&info,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    quint8 type=24;
    out<<is;
    qDebug()<<tr("type%1").arg(type);
    send_Data(type,info);
}

void TcpSocket::fendUser(QByteArray data)
{//25
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QString userid,id;
    in>>userid>>id;
    QByteArray user=getUserInfo(id);
    quint8 type=25;
    send_Data(type,user);
}

void TcpSocket::userFrinedFend(QByteArray data)
{//26
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QString id;
    in>>id;
    emit userLoginInfo(id);
    quint8 type=26;
    QByteArray info=getUserFrined(id);
    send_Data(type,info);
    isLogin=true;
    userUnreadInfo(id);
}

void TcpSocket::isManySpeakOpen()
{//27
    emit publicUser();
}

void TcpSocket::forget(QByteArray data)
{//28
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QString id,m_1,m_2,pass;
    in>>id;
    QSqlRecord re=m_data->userSelect(id);
    QByteArray info;
    QDataStream out(&info,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    quint8 type=28;
    if(re.isEmpty())
    {
        out<<false<<tr("没有该用户");
        send_Data(type,info);
        return;
    }
    in>>m_1>>m_2;
    QString M_1,M_2;
    M_1=re.value("user_secret_1").toString();
    M_2=re.value("user_secret_2").toString();
    if(M_1==m_1&&M_2==m_2)
    {
        in>>pass;
        m_data->userPassForget(id,pass);
        out<<true;
        send_Data(type,info);
        return;
    }
    else
    {
        out<<false<<tr("密保错误");
        send_Data(type,info);
        return;
    }
}
