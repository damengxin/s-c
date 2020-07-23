#ifndef USERCHAT_H
#define USERCHAT_H

#include <QWidget>
#include <QDateTime>

#include "qmemoji.h"

namespace Ui {
class userChat;
}

class userChat : public QWidget
{
    Q_OBJECT

    static QString times(){return QDateTime::currentDateTime()
                .toString("yyyy.MM.dd hh:mm ");}
public:
    explicit userChat(QString name,QWidget *parent = nullptr);
    ~userChat();
    void setUserNameId(QString name);
    void setChat(QString &time,QString &msg);

public slots:
    void up(QString name);

private slots:
    void emojiMsg(QString s);
    void visible();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

signals:
    void udpInfo(quint8 type,QString msg);
    void msgInfo(QString msg);

private:
    Ui::userChat *ui;
    QString m_userName;
    QMEmoji *m_emoji=nullptr;
};

#endif // USERCHAT_H
