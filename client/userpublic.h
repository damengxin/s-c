#ifndef USERPUBLIC_H
#define USERPUBLIC_H

#include <QWidget>
#include <QUdpSocket>
#include <QHostAddress>
#include <QDateTime>

#include "qmemoji.h"

namespace Ui {
class userPublic;
}

class userPublic : public QWidget
{
    Q_OBJECT
    static QString times(){return QDateTime::currentDateTime()
                .toString("yyyy.MM.dd hh:mm ");}


public:
    explicit userPublic(QString id,QString name,
                        QWidget *parent = nullptr);
    ~userPublic();
    bool getPublicOpen(){return isOpen;}
    void iniSocket();
    void setIpPort(QString ip,quint16 port);
    void readPublic();
    void sendPublic(quint8 type,QByteArray info);
    void userAddPublic(QByteArray info);           //41
    void userGone(QByteArray info);                //42
    void msgPublic(QByteArray info);               //43

public slots:
    void up(QString naem);
    void isPublicOpen(QByteArray info);

private slots:
    void emojiMsg(QString s);
    void visible();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();
signals:
    void clear();

private:
    Ui::userPublic *ui;
    QHostAddress m_address;              //主机Tcpip和port
    QString m_id;
    QString m_name;
    QString m_ip;
    bool isOpen;                         //群聊是否开启
    QUdpSocket *m_publicSocket=nullptr;  //群聊
    QList<QString> listId;               //群聊用户id
    QList<QString> listName;             //群聊用户名
    QHostAddress m_publicAddress;
    quint16 m_publicPort;
    QMEmoji *m_emoji=nullptr;
};

#endif // USERPUBLIC_H
