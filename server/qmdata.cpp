#include "qmdata.h"
#include <QDebug>
#include <QPixmap>

QString QMData::random()
{//生成8位 随机账号
    QTime t=QTime::currentTime();
    qsrand(t.msec());
    int i;
    QString s;
    for(int n=0;n<8;n++)
    {
        i=qrand()%9+1;
        switch (i) {
        case 0: s+="0";break;
        case 1: s+="1";break;
        case 2: s+="2";break;
        case 3: s+="3";break;
        case 4: s+="4";break;
        case 5: s+="5";break;
        case 6: s+="6";break;
        case 7: s+="7";break;
        case 8: s+="8";break;
        case 9: s+="9";break;
        }
    }
    return s;
}

QMData::QMData(QString databaseType,QString connectName,QObject *parent)
               :QObject(parent)
{
    DB=QSqlDatabase::addDatabase(databaseType,connectName);
    DB.setDatabaseName(databaseName);
}

QMData::QMData(QString databaseType, QObject *parent):
    QObject(parent)
{
    DB=QSqlDatabase::addDatabase(databaseType);
    DB.setDatabaseName(databaseName);
}

void QMData::remove(QString connectName)
{
    DB.removeDatabase(connectName);
}

QSqlRecord QMData::userSelect(QString &id)
{//通过用户id查询
    QSqlRecord re;
    if(!DB.open())
    {
        QString error=tr("用户%1查询失败\n原因%2").arg(id)
                .arg("数据库未打开");
        emit dataError(error);
        return re;
    }
    QSqlQuery qu(DB);
    QString str;
    qu.prepare("select * from m_user where user_id=:id");
    qu.bindValue(":id",id);
    qu.exec();
    qDebug()<<"data select "<<id;
    if(!qu.isActive())
    {
        QString error=tr("用户%1查询失败\n原因%2").arg(id)
                .arg(qu.lastError().text());
        emit dataError(error);
        DB.close();
        return re;
    }
    if(qu.next())
    {
        re=qu.record();
    }
    DB.close();
    return re;
}

bool QMData::userModify(QByteArray &userData)
{//通过用户id修改用户信息 修改成功返回true
    QDataStream in(&userData,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QString id,name,area,motto;
    QByteArray photo;
    bool sex;
    int age;
    in>>id>>name>>sex>>age>>area>>photo>>motto;
    qDebug()<<id<<name<<motto;
    if(!DB.open())
    {
        QString error=tr("用户%1信息修改失败\n原因%2").arg(id)
                .arg("数据库未打开");
        emit dataError(error);
        return false;
    }
    QSqlQuery qu(DB);
    qu.prepare("update m_user set user_name=:name,"
               "user_sex=:sex,user_age=:age,"
               "user_area=:area,user_photo=:photo,"
               "user_motto=:motto "
               " where user_id=:id");
    qu.bindValue(":name",name);
    qu.bindValue(":sex",sex);
    qu.bindValue(":age",age);
    qu.bindValue(":area",area);
    qu.bindValue(":photo",photo);
    qu.bindValue(":motto",motto);
    qu.bindValue(":id",id);
    qu.exec();
    if(qu.isActive())//成功
    {
        DB.close();
        return true;
    }
    QString error=tr("用户%1信息修改失败\n原因%2").arg(id)
            .arg(qu.lastError().text());
    emit dataError(error);
    return false;
}

QString QMData::userIncrease(QByteArray &userData)
{//增加一个用户，成功返回一个账号,失败返回空字符串
    if(!DB.open())
    {
        QString error=tr("用户账号申请失败\n原因%2")
                .arg("数据库未打开");
        emit dataError(error);
        return "";
    }
    QSqlQuery qu(DB);
    QSqlRecord record;
    QString id=QMData::random();
    //查询是否有该用户
    qu.prepare("select * from m_user where user_id=:id");
    qu.bindValue(":id",id);
    qu.exec();
    if(qu.isActive())//执行成功
    {
        record=qu.record();
        if (record.isNull("user_id"))//字段为空
        {
            QDataStream in(&userData,QIODevice::ReadOnly);
            in.setVersion(QDataStream::Qt_5_13);
            //昵称，密码，地区，签名,密保1,2
            QString name,area,pass,motto,s_1,s_2;
            QByteArray photo;//照片
            bool sex;//性别
            int age;//年龄
            in>>name>>sex>>age>>area>>pass>>photo>>motto>>s_1>>s_2;
            qDebug()<<"new 3"<<id<<name<<sex<<age<<pass<<area<<s_1;
            qu.prepare("insert into m_user (user_id,user_name,user_sex,"
                       "user_age,user_area,user_password,user_photo,"
                       "user_motto,ip,isonline,user_secret_1,"
                       "user_secret_2) values(:id,:name,:sex,:age,:area,"
                       ":pass,:photo,:motto,false,false,:s_1,:s_2)");
            qu.bindValue(":id",id);
            qu.bindValue(":name",name);
            qu.bindValue(":sex",sex);
            qu.bindValue(":age",age);
            qu.bindValue(":area",area);
            qu.bindValue(":pass",pass);
            qu.bindValue(":photo",photo);
            qu.bindValue(":motto",motto);
            qu.bindValue(":s_1",s_1);
            qu.bindValue(":s_2",s_2);
            qu.exec();
            if(qu.isActive())
            {
                qDebug()<<"new 4";
                DB.close();
                return id;
            }
        }
    }
    QString error=tr("用户申请账号失败\n原因%2")
            .arg(qu.lastError().text());
    emit dataError(error);
    DB.close();
    return "";
}

bool QMData::userPassModify(QByteArray &userData)
{
    QDataStream in(&userData,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QString id,pass;
    in>>id>>pass;
    if(!DB.open())
    {
        QString error=tr("用户%1信息修改失败\n原因%2").arg(id)
                .arg("数据库未打开");
        emit dataError(error);
        return false;
    }

    QSqlQuery qu(DB);
    qu.prepare("update m_user set user_password=:pass "
               " where user_id=:id");
    qu.bindValue(":pass",pass);
    qu.bindValue(":id",id);
    qu.exec();
    if(qu.isActive())//成功
    {
        DB.close();
        return true;
    }
    QString error=tr("用户%1信息修改失败\n原因%2").arg(id)
            .arg(qu.lastError().text());
    emit dataError(error);
    return false;
}

bool QMData::userPassForget(QString &id,QString &pass)
{
    if(!DB.open())
    {
        QString error=tr("用户%1信息修改失败\n原因%2").arg(id)
                .arg("数据库未打开");
        emit dataError(error);
        return false;
    }
    QSqlQuery qu(DB);
    qu.prepare("update m_user set user_password=:pass "
               " where user_id=:id");
    qu.bindValue(":pass",pass);
    qu.bindValue(":id",id);
    qu.exec();
    if(qu.isActive())//成功
    {
        DB.close();
        return true;
    }
    QString error=tr("用户%1信息修改失败\n原因%2").arg(id)
            .arg(qu.lastError().text());
    emit dataError(error);
    return false;
}

void QMData::userConnect(QString &id,QString &ip)
{
    if(!DB.open())
    {
        QString error=tr("用户%1保存ip失败\n原因%2").arg(id)
                .arg("数据库未打开");
        emit dataError(error);
        return;
    }
    QSqlQuery qu(DB);
    qu.prepare("update m_user "
               " set isonline=:isonline,ip=:ip where user_id=:id");
    qu.bindValue(":isonline",true);
    qu.bindValue(":ip",ip);
    qu.bindValue(":id",id);
    qu.exec();
    DB.close();
}

void QMData::userDisconnect(QString &id)
{
    if(!DB.open())
    {
        QString error=tr("用户%1离线ip修改失败\n原因%2").arg(id)
                .arg("数据库未打开");
        emit dataError(error);
        return;
    }
    QSqlQuery qu(DB);
    qu.prepare("update m_user "
               " set isonline=:isonline,ip=:ip where user_id=:id");
    qu.bindValue(":isonline",false);
    qu.bindValue(":ip","");
    qu.bindValue(":id",id);
    qu.exec();
    DB.close();
}

QList<QSqlRecord> QMData::buddySelect(QString &id)
{//通过用户id查询
        QList<QSqlRecord> list;
        QList<QString> listid;
        if(!DB.open())
        {
            QString error=tr("用户%1好友查询失败\n原因%2").arg(id)
                    .arg("数据库未打开");
            emit dataError(error);
            return list;
        }
        QSqlQuery qu(DB);
        QString friendid;
        qu.prepare("select * from m_friend where userid=:id");
        qu.bindValue(":id",id);
        qu.exec();
        if(!qu.isActive())
        {
            QString error=tr("用户%1好友查询失败\n原因%2").arg(id)
                    .arg(qu.lastError().text());
            emit dataError(error);
            DB.close();
            return list;
        }
        while(qu.next())
        {
            listid.append(qu.value("friendid").toString());
        }
        for(int i=0;i<listid.count();i++)
        {
            friendid=listid.at(i);
            qu.prepare("select * from m_user where user_id=:id");
            qu.bindValue(":id",friendid);
            qu.exec();
            if(qu.next())
            {
                list.append(qu.record());
            }
        }
        DB.close();
        return list;
}


bool QMData::buddyIncrease(QString &userid,QString &frinedId)
{//添加一个用户好友，成功返回false
    if(!DB.open())
    {
        QString error=tr("用户%1添加好友失败\n原因%2").arg(userid)
                .arg("数据库未打开");
        emit dataError(error);
        return false;
    }
    QSqlQuery qu(DB);
    qu.prepare("insert into m_friend (userid,friendid) "
               " values(:id,:m_id)");
    qu.bindValue(":id",userid);
    qu.bindValue(":m_id",frinedId);
    qu.exec();
    if(!qu.isActive())//失败
    {
        QString error=tr("用户%1添加好友失败\n原因%2").arg(userid)
                .arg(qu.lastError().text());
        emit dataError(error);
        DB.close();
        return false;
    }
    DB.close();
    return true;
}

bool QMData::saveInfo(QString userid,quint8 type,QByteArray &offlineData)
{//如果用户不在线，保存未读信息
    if(!DB.open())
    {
        QString error=tr("保存用户离线信息失败\n原因%1")
                .arg("数据库未打开");
        emit dataError(error);
        return false;
    }
    QSqlQuery qu(DB);
    qu.prepare("insert into m_unread (user_id,"
               "info_type,data) values(:id,:type,"
               ":data)");
    qu.bindValue(":id",userid);
    qu.bindValue(":type",type);
    qu.bindValue(":data",offlineData);
    qu.exec();
    if(!qu.isActive())//失败
    {
        QString error=tr("保存用户离线信息失败\n原因%1")
                .arg(qu.lastError().text());
        emit dataError(error);
        DB.close();
        return false;
    }
    DB.close();
    return true;
}

QList<QSqlRecord> QMData::getInfo(QString &id)
{//通过id查询所有未读信息
        QList<QSqlRecord> list;
        if(!DB.open())
        {
            QString error=tr("获取用户离线信息失败\n原因%1")
                    .arg("数据库未打开");
            emit dataError(error);
            return list;
        }
        QSqlQuery qu(DB);
        QString str;
        qu.prepare("select * from m_unread where user_id=:id");
        qu.bindValue(":id",id);
        qu.exec();
        if(!qu.isActive())
        {
            QString error=tr("获取用户离线信息失败\n原因%1")
                    .arg(qu.lastError().text());
            emit dataError(error);
            DB.close();
            return list;
        }
        while(qu.next())
        {
            list.append(qu.record());
        }
        DB.close();
        return list;
}

bool QMData::clearInfo(QString &id)
{//根据id删除未读信息
    if(!DB.open())
    {
        QString error=tr("删除用户离线信息失败\n原因%1")
                .arg("数据库未打开");
        emit dataError(error);
        return false;
    }
    QSqlQuery qu(DB);
    qu.prepare("delete from m_unread where user_id=:id");
    qu.bindValue(":id",id);
    qu.exec();
    if(qu.isActive())
    {
        DB.close();
        return true;
    }
    QString error=tr("删除用户离线信息失败\n原因%1")
            .arg(qu.lastError().text());
    qDebug()<<"saveInfo3"<<error;
    emit dataError(error);
    DB.close();
    return false;
}
