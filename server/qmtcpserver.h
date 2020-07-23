#ifndef QMTCPSERVER_H
#define QMTCPSERVER_H

#include <QTcpServer>
#include <QThread>
//#include <QMetaType>

#include "tcpsocket.h"

//Q_DECLARE_METATYPE(qintptr)
class QMTcpServer:public QTcpServer
{

    Q_OBJECT
protected:
    void incomingConnection(qintptr handle);

public:
    TcpSocket *nextPendingConnection();
    QMTcpServer(QObject *parent=nullptr);
    void addConnection(TcpSocket *socket);

signals:
    void setHanlde(qintptr handle);

private:
    QList<TcpSocket*> Connected;
};

#endif // QMTCPSERVER_H
