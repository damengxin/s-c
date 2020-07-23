#include "findfrined.h"
#include "ui_findfrined.h"
#include "userinfo.h"
#include <QDebug>

findFrined::findFrined(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::findFrined)
{
    ui->setupUi(this);
}

findFrined::~findFrined()
{
    delete ui;
}

void findFrined::setFineUser(QByteArray data,bool b)
{
    if(data.size()==0)
    {
        QListWidgetItem *item=new QListWidgetItem;
        item->setText(tr("没有该用户！"));
        ui->listWidget->addItem(item);
        return;
    }
    userInfo *user=new userInfo;
    connect(user,&userInfo::add,
            this,&findFrined::add);
    connect(user,&userInfo::clear,
            this,&findFrined::clear_Button);
    user->setUser(data);
    user->addVisible(true);
    user->addEnabled(b);
    QListWidgetItem *item=new QListWidgetItem;
    item->setSizeHint(QSize(ui->listWidget->width(),90));
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item,user);
}

void findFrined::setUnreadInfo(QByteArray data)
{
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
        connect(user,&userInfo::agree,
                this,&findFrined::agree);
        connect(user,&userInfo::clear,
                this,&findFrined::clear_Button);
        user->setUser(da);
        user->agreeVisible(true);
        QListWidgetItem *item=new QListWidgetItem;
        item->setSizeHint(QSize(ui->listWidget->width(),90));
        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item,user);
    }

}

void findFrined::setFineVisible(bool b)
{
    ui->label->setVisible(b);
    ui->lineEdit->setVisible(b);
    ui->pushButton->setVisible(b);
}

void findFrined::on_pushButton_clicked()
{
    QString id=ui->lineEdit->text();
    ui->lineEdit->clear();
    ui->listWidget->clear();
    if(id.isEmpty())
    {
        return;
    }
    quint8 type=25;
    emit find(type,id);
}

void findFrined::clear_Button()
{
    userInfo *user=qobject_cast<userInfo*>(sender());
    for (int i=0;i<ui->listWidget->count();i++)
    {
     QListWidgetItem *item=ui->listWidget->item(i);
     userInfo *send=qobject_cast<userInfo*>(
                 ui->listWidget->itemWidget(item));
     if(user==send)
     {
         ui->listWidget->removeItemWidget(item);
         ui->listWidget->takeItem(i);
         delete send;
         delete item;
     }
    }
}
