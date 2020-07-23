#include "qmuser.h"
#include "ui_qmuser.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QListWidgetItem>
#include <QDebug>
#include <QThread>
#include <QCursor>
#include <QMessageBox>

void QMUser::closeEvent(QCloseEvent *event)
{
    emit dis();
}

QMUser::QMUser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QMUser)
{
    ui->setupUi(this);
    m_panel=new userPanel;
    m_panel->setWindowTitle(tr("好友详情"));
    connect(ui->photo,&QMLabel::clicket,
            m_panel,&userPanel::show);
    connect(m_panel,&userPanel::userInfo,
            this,&QMUser::send_tcp);
    ui->listWidget->setIconSize(QSize(ui->listWidget->width()/6,90));
}

QMUser::~QMUser()
{
    delete ui;
}

void QMUser::setIpPort(QString ip, quint16 port)
{
    m_ip=ip;
    m_port=port;
}

void QMUser::setUser(QByteArray &data)
{
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QString id,name,area,motto;
    bool sex;
    int age;
    QByteArray photo;
    in>>id>>name>>sex>>age>>area>>photo>>motto;
    m_id=id;
    ui->name->setText(name);
    ui->motto->setText(motto);
    QPixmap map;
    map.loadFromData(photo);
    ui->photo->setPixmap(map);
    m_panel->setUser(data);
    m_panel->setmodify(true);
}

void QMUser::userUnread(QByteArray data)
{//1
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    int count=0;
    QList<QByteArray> list;
    QByteArray da;
    in>>count>>list;
    for (int i=0;i<count;i++)
    {
        da=list.at(i);
        udp_readInfo(da);
    }
}

void QMUser::systemUnread(QByteArray data)
{//3
    if(m_info==nullptr)
    {
        m_info=new findFrined;
        m_info->setWindowTitle(tr("系统信息"));
        m_info->setFineVisible(false);
        connect(m_info,&findFrined::agree,
                this,&QMUser::frined);
    }
    m_info->setUnreadInfo(data);
    ui->pushButton->setIcon(QIcon(":/icon/yxx.png"));
}

void QMUser::upList(QByteArray data)
{//5
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    userInfo *user=new userInfo;
    user->setUser(data);
    QListWidgetItem *item=new QListWidgetItem;
    item->setSizeHint(QSize(ui->listWidget_2->width()/6,90));
    ui->listWidget_2->addItem(item);
    ui->listWidget_2->setItemWidget(item,user);
}

void QMUser::upUserInfo(QByteArray data)
{//23
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    bool b;
    in>>b;
    m_panel->setUp(b);
    if(b)
    {
        QPixmap map;
        map.loadFromData(m_panel->getUserPhoto());
        ui->photo->setPixmap(map);
        ui->name->setText(tr("%1").arg(m_panel->getUserNmae()));
        ui->motto->setText(tr("%1").arg(m_panel->getUserMotto()));
        emit up(m_panel->getUserNmae());
    }
}

void QMUser::upPassword(QByteArray data)
{//24
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    bool b;
    in>>b;
    if(b)
    {
        QMessageBox::information(this,tr("密码修改"),tr("密码修改成功\n请重新登录"));
        ui->listWidget->clear();
        ui->listWidget_2->clear();
        m_chats.clear();
        delete m_panel;
        delete m_find;
        delete m_info;
        delete m_pass;
        m_panel=nullptr;
        m_find=nullptr;
        m_info=nullptr;
        m_pass=nullptr;
        emit renewLogin();
    }
    else
    {
        QMessageBox::information(this,tr("密码修改"),tr("密码失败"));
    }
}

void QMUser::findUser(QByteArray data)
{//25
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QString id;
    in>>id;
    if(id==m_id)
    {
        m_find->setFineUser(data,false);
        return;
    }
    for(int i=0;i<ui->listWidget_2->count();i++)
    {
        QListWidgetItem *item=ui->listWidget_2->item(i);
        userInfo *user=qobject_cast<userInfo*>
                (ui->listWidget_2->itemWidget(item));
        if(id==user->getUserId())
        {
            m_find->setFineUser(data,false);
            return;
        }
    }
    m_find->setFineUser(data,true);
}

void QMUser::userFrined(QByteArray data)
{//26
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    int count=0;
    QList<QByteArray> list;
    in>>count>>list;
    for(int i=0;i<count;i++)
    {
        QByteArray da;
        da=list.at(i);
        userInfo *user=new userInfo;
        user->setUser(da);
        QListWidgetItem *item=new QListWidgetItem;
        item->setSizeHint(QSize(ui->listWidget_2->width()/6,90));
        ui->listWidget_2->addItem(item);
        ui->listWidget_2->setItemWidget(item,user);
    }
}

void QMUser::udpInfo(quint8 type, QString msg)
{
    userChat *chat=qobject_cast<userChat*>(sender());
    QString id=m_chats.key(chat);
    QByteArray info;
    QDataStream out(&info,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out<<id<<m_id<<msg;
    emit send_udp(type,info);
}

void QMUser::udp_readInfo(QByteArray info)
{qDebug()<<"udp read client";
    QDataStream in(&info,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QString id,time,msg;
    in>>id>>time>>msg;
    userChat *chat;
    chat=m_chats.value(id);
    if(chat==nullptr)
    {
        for(int i=0;i<ui->listWidget_2->count();i++)
        {
            QListWidgetItem *item=ui->listWidget_2->item(i);
            userInfo *user=qobject_cast<userInfo*>
                    (ui->listWidget_2->itemWidget(item));
            if(id==user->getUserId())
            {
                QListWidgetItem *item=new QListWidgetItem;

                item->setText(tr("%1").arg(user->getUserNmae()));
                item->setData(9,user->getUserId());
                QPixmap map;
                map.loadFromData(user->getPhoto());
                item->setIcon(QIcon(map));
                ui->listWidget->insertItem(0,item);
                chat=new userChat(ui->name->text());
                chat->setUserNameId(user->getUserNmae());
                m_chats.insert(user->getUserId(),chat);
                connect(chat,&userChat::udpInfo,
                        this,&QMUser::udpInfo);
                connect(chat,&userChat::msgInfo,
                        this,&QMUser::msgInfo);
                connect(this,&QMUser::up,chat,&userChat::up);
            }
        }
    }
    chat->setChat(time,msg);
}

void QMUser::frined(quint8 type, QString id)
{
    QByteArray info;
    QDataStream out(&info,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out<<m_id<<id;
    emit send_tcp(type,info);
}

void QMUser::user_data(QByteArray data)
{
    quint8 index;
    int size;
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QByteArray da;
    in>>index>>size>>da;
    data.clear();
    qDebug()<<tr("client %1").arg(index)<<da.size();
    switch (index)
    {
    case 1:userUnread(da);return;
    case 3:systemUnread(da);return;
    case 5:upList(da);return;
    case 23:upUserInfo(da);return;
    case 24:upPassword(da);return;
    case 25:findUser(da);return;
    case 27:emit isOpen(da);return;
    }
}

void QMUser::msgInfo(QString msg)
{//在信息面板显示最后一句话
    userChat *chat=qobject_cast<userChat*>(sender());
    QString id=m_chats.key(chat);
    for(int i=0;i<ui->listWidget->count();i++)
    {
        QListWidgetItem *item=ui->listWidget->item(i);
        if(id==item->data(9))
        {
            QString text=chat->windowTitle();
            item->setText(tr("%1\n%2")
                          .arg(text)
                          .arg(msg));
        }
    }
}

void QMUser::pass(QString pass)
{
    QByteArray info;
    QDataStream out(&info,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out<<m_id<<pass;
    quint8 type=24;
    emit send_tcp(type,info);
}

void QMUser::on_listWidget_customContextMenuRequested(const QPoint &pos)
{//删除信息
    QListWidgetItem *item=ui->listWidget->itemAt(pos);
    int row=ui->listWidget->row(item);
    QMenu *menu=new QMenu(ui->listWidget);
    QAction *act=new QAction(tr("删除"),ui->listWidget);
    if(item==nullptr)
        act->setEnabled(false);
    menu->addAction(act);
    connect(act,&QAction::triggered,[&]{
        ui->listWidget->takeItem(row);
        delete item;
    });
    menu->exec(QCursor::pos());
    delete menu;
    delete act;
}

void QMUser::on_listWidget_2_itemDoubleClicked(QListWidgetItem *item)
{
    userInfo *user=qobject_cast<userInfo*>(ui->listWidget_2->itemWidget(item));
    userChat *chat;
    chat=m_chats.value(user->getUserId());
    if(chat==nullptr)
    {
        chat=new userChat(ui->name->text());
        chat->setUserNameId(user->getUserNmae());
        m_chats.insert(user->getUserId(),chat);
        connect(chat,&userChat::udpInfo,
                this,&QMUser::udpInfo);
        QListWidgetItem *item=new QListWidgetItem;
        connect(chat,&userChat::msgInfo,
                this,&QMUser::msgInfo);
        connect(this,&QMUser::up,chat,&userChat::up);
        item->setText(tr("%1").arg(user->getUserNmae()));
        item->setData(9,user->getUserId());
        QPixmap map;
        map.loadFromData(user->getPhoto());
        item->setIcon(QIcon(map));
        ui->listWidget->insertItem(0,item);
    }
    chat->show();
}

void QMUser::on_pushButton_2_clicked()
{
    if(m_find==nullptr)
    {
        m_find=new findFrined;
        m_find->setWindowTitle(tr("添加新朋友"));
        connect(m_find,&findFrined::find,
                this,&QMUser::frined);
        connect(m_find,&findFrined::add,
                this,&QMUser::frined);
    }
    m_find->show();
    m_find->setFocus();
}

void QMUser::on_pushButton_clicked()
{
    if(m_info==nullptr)
    {
        m_info=new findFrined;
        m_info->setWindowTitle(tr("系统信息"));
        m_info->setFineVisible(false);
        connect(m_info,&findFrined::agree,
                this,&QMUser::frined);
    }
    m_info->show();
    m_info->setFocus();
    ui->pushButton->setIcon(QIcon(":/icon/wxx.png"));
}

void QMUser::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString id=item->data(9).toString();
    userChat *chat;
    chat=m_chats.value(id);
    chat->show();
    int row=ui->listWidget->row(item);
    ui->listWidget->takeItem(row);
    ui->listWidget->insertItem(0,item);
}

void QMUser::on_pushButton_4_clicked()
{
    if(m_pass==nullptr)
    {
        m_pass=new upPass;
        m_pass->setWindowTitle(tr("修改密码"));
        connect(m_pass,&upPass::pass,
                this,&QMUser::pass);
    }
    m_pass->show();
}

void QMUser::on_pushButton_3_clicked()
{
    if(m_public==nullptr)
    {
        m_public=new userPublic(m_id,ui->name->text());
        m_public->setWindowTitle(tr("群聊"));
        m_public->setIpPort(m_ip,m_port);
        connect(this,&QMUser::isOpen,
                m_public,&userPublic::isPublicOpen);
        connect(this,&QMUser::up,m_public,&userPublic::up);
    }
    if(!m_public->getPublicOpen())
    {
        QByteArray info;
        quint8 type=27;
        send_tcp(type,info);
    }
    m_public->show();
}
