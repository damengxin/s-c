#ifndef QMLOGIN_H
#define QMLOGIN_H

#include <QDialog>
#include <QThread>
#include <QUdpSocket>

#include "tcpsocket.h"
#include "qmuser.h"
#include "newuser.h"
#include "forget.h"

namespace Ui {
class QMLogin;
}

class QMLogin : public QDialog
{
    Q_OBJECT
private:
    void closeEvent(QCloseEvent *event);

public:
    explicit QMLogin(QWidget *parent = nullptr);
    ~QMLogin();
    QString encrypt(QString &str);
    void userLoginCheck(QByteArray data);  //21
    void userFrined(QByteArray data);      //26
    void newUserId(QByteArray data);

public slots:
    void read_Data(QByteArray data);
    void error(QByteArray data);
    void hostConnect();
    void dis();
    void udp_send(quint8 type,QByteArray info);
    void udp_read();
    void renewLogin();

signals:
    void connectHost(QString ip,quint16 port);
    void disConnect();
    void send(quint8 type,QByteArray info);
    void read_udpInfo(QByteArray);
    void tcpData(QByteArray);

private slots:
    void on_pushButton_5_clicked();

    void on_pushButton_2_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_lineEdit_2_textChanged(const QString &arg1);

    void on_pushButton_6_clicked();

    void on_ip_textChanged(const QString &arg1);

    void on_port_textChanged(const QString &arg1);

    void on_pushButton_7_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_4_pressed();

    void on_pushButton_3_clicked();

    void on_pushButton_3_pressed();

private:
    Ui::QMLogin *ui;
    QHostAddress m_address; //主机Tcpip和port
    QString m_ip;
    quint16 m_port;
    TcpSocket *m_socket=nullptr;
    QMUser *m_client=nullptr;

    bool isSocket;          //该连接是否有用
    QString userId;
    QThread *thread=nullptr;
    newUser *m_newUser=nullptr;
    QUdpSocket *m_udp=nullptr;
    forget *m_forget=nullptr;
};

#endif // QMLOGIN_H
