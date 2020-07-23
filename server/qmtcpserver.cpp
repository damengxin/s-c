#include "qmtcpserver.h"

QMTcpServer::QMTcpServer(QObject *parent):
    QTcpServer(parent)
{
}

TcpSocket *QMTcpServer::nextPendingConnection()
{
    if (this->Connected.isEmpty())
            return nullptr;
    return this->Connected.takeFirst();
}

void QMTcpServer::addConnection(TcpSocket *socket)
{
    Connected.append(socket);
}

void QMTcpServer::incomingConnection(qintptr handle)
{
    QThread *thread=new QThread;
    thread->start();
    TcpSocket *socket=new TcpSocket;
    socket->moveToThread(thread); 
    QMetaObject::invokeMethod(socket,[socket,handle]
    {socket->setSocketDescriptor(handle);},
        Qt::BlockingQueuedConnection);
    connect(socket,&TcpSocket::disconnected,
            socket,&TcpSocket::userDisConnect);
    connect(socket,&TcpSocket::readyRead,
            socket,&TcpSocket::read_Data);
    addConnection(socket);
}
