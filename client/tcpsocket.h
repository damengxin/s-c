#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QTcpSocket>
#include <QByteArray>
#include <QThread>
#include <QTimer>


class TcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    TcpSocket(QObject *parent=nullptr);
    ~TcpSocket();


public slots:
    void receive_Data();
    void send_Data(quint8 type,QByteArray info);
    void dataParsing();
    void connectHost(QString ip,quint16 port);
    void disConnect();
    void timerStart();
    void heart();

signals:
///////////////////UI操作///////////////////////////////
    void read_Data(QByteArray data);
    void clientError(QByteArray data);

private:
    QByteArray buffer;      //存放socket信息
    QByteArray buffer_2;    //组包
    QTimer *m_time;         //心跳
    QTimer *m_dataTime;     //处理数据
};

#endif // TCPSOCKET_H
