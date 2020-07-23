#include "tcpsocket.h"
#include <QDataStream>
#include <QHostAddress>


TcpSocket::TcpSocket(QObject *parent):
    QTcpSocket(parent)
{
    m_time=new QTimer(this);
    connect(m_time,&QTimer::timeout,
            this,&TcpSocket::heart);
    connect(this,&TcpSocket::connected,
            this,&TcpSocket::timerStart);
    m_dataTime=new QTimer(this);
    connect(m_dataTime,&QTimer::timeout,
            this,&TcpSocket::dataParsing);
}

TcpSocket::~TcpSocket()
{
}

void TcpSocket::dataParsing()
{
    if(buffer_2.size()==0)
    {
        m_dataTime->stop();
        return;
    }
    quint8 index;
    int size;
    QDataStream in(&buffer_2,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    in>>index>>size;
    qDebug()<<"处理信息"<<index<<size<<buffer_2.size();
    if(index==1||index==3||index==5||index==21||
            index==22||index==23||index==24||index==25||
            index==26||index==27||index==28)
    {//login
        if(size<buffer_2.size())
        {
            QByteArray pack_1=buffer_2.left(size);
            QByteArray pack_2=buffer_2.right(buffer_2.size()-pack_1.size());
            buffer_2=pack_2;
            emit read_Data(pack_1);
        }
        else if(size==buffer_2.size())
        {
            emit read_Data(buffer_2);
            buffer_2.clear();
            return;

        }
    }
    else
    {qDebug()<<"错误信息"<<index;
        emit clientError(buffer_2);
        buffer_2.clear();
        return;
    }
}


void TcpSocket::receive_Data()
{//读取
    buffer.clear();
    buffer=this->readAll();
    buffer_2+=buffer;
    quint8 index;
    QDataStream in(&buffer_2,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    in>>index;
    qDebug()<<"read   "<<index;
    if(!m_dataTime->isActive())
        m_dataTime->start();
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
    m_time->start();
    qDebug()<<"send "<<type<<info.size();
}

void TcpSocket::connectHost(QString ip, quint16 port)
{

    QHostAddress address;
    address.setAddress(ip);
    this->abort();
    this->connectToHost(address,port);

}

void TcpSocket::disConnect()
{
    this->abort();
    this->disconnectFromHost();
    m_time->stop();
    delete m_time;
}

void TcpSocket::timerStart()
{
    m_time->start(1000*10);
}

void TcpSocket::heart()
{
    QByteArray info;
    quint8 type=7;
    send_Data(type,info);
}

