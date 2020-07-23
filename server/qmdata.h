#ifndef QMDATA_H
#define QMDATA_H

#include <QtSql>
#include <QTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QByteArray>

///构造函数确定数据库类型使用socket描述符作为连接名
///
///userSelect通过用户id查询,信号dataError发送错误信息，错误返回空记录
///userModify通过用户id修改用户信息,修改成功返回true
///userIncrease增加一个用户，成功返回一个账号,失败返回空字符串
///userConnet保存用户的ip地址，改变在线状态
///userDisconnect清除用户ip，改变在线状态
///
///buddySelect通过用户id查询,错误返回空字符串
///buddyIncrease添加一个用户好友，成功返回true
///
///saveInfo如果用户不在线，保存好友未读信息，成功返回true
///getInfo根据id获取好友未读信息
///删除好友未读信息，成功返回true
///如果用户不在线，保存系统未读信息，成功返回true
///根据id获取系统未读信息
///删除系统未读信息信息，成功返回true


class QMData :public QObject
{
    Q_OBJECT

public:
    QMData(QString databaseType,QString connectName,
           QObject *parent=nullptr);
    QMData(QString databaseType,QObject *parent=nullptr);

    void remove(QString connectName);

    static QString random();
//////////////////////////用户/////////////////////////////////
    QSqlRecord userSelect(QString &id);
    bool userModify(QByteArray &userData);
    QString userIncrease(QByteArray &userData);
    bool userPassModify(QByteArray &userData);
    bool userPassForget(QString &id,QString &pass);
    void userConnect(QString &id,QString &ip);
    void userDisconnect(QString &id);
///////////////////////////用户好友/////////////////////////////
    QList<QSqlRecord> buddySelect(QString &id);
    bool buddyIncrease(QString &userid,QString &frinedId);
/////////////////////////////未读信息///////////////////////////
    bool saveInfo(QString id,quint8 type,
                  QByteArray &offlineData);
    QList<QSqlRecord> getInfo(QString &id);
    bool clearInfo(QString &id);

signals:
    void dataError(QString error);
private:
    QSqlDatabase DB;
    QString databaseName="da.db";
};

#endif // QMDATA_H
