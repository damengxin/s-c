#include "userinfo.h"
#include "ui_userinfo.h"
#include <QDebug>
#include <QPixmap>

userInfo::userInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::userInfo)
{
    ui->setupUi(this);
    ui->pushButton->setVisible(false);
    ui->pushButton_2->setVisible(false);
    ui->pushButton_3->setVisible(false);
    ui->pushButton_4->setVisible(false);
    m_panel=new userPanel;
}

userInfo::~userInfo()
{
    delete ui;
}

void userInfo::setUser(QByteArray &user)
{
    QDataStream in(&user,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QString id,name,area,motto;
    bool sex;
    int age;
    QByteArray photo;
    in>>id>>name>>sex>>age>>area>>photo>>motto;
    ui->name->setText(name);
    QPixmap map;
    map.loadFromData(photo);
    ui->photo->setPixmap(map);
    ui->motto->setText(motto);
    m_panel->setUser(user);
}

void userInfo::setmodify(bool n)
{
    m_panel->setmodify(n);
}

void userInfo::addVisible(bool b)
{
    ui->pushButton->setVisible(b);
    ui->pushButton_2->setVisible(b);
}

void userInfo::addEnabled(bool b)
{
    ui->pushButton->setEnabled(b);
}

void userInfo::agreeVisible(bool b)
{
    ui->pushButton_3->setVisible(b);
    ui->pushButton_4->setVisible(b);
}

void userInfo::on_photo_clicket()
{
    m_panel->show();
}

void userInfo::on_pushButton_clicked()
{
    quint8 type=2;
    emit add(type,getUserId());
    emit clear();
}

void userInfo::on_pushButton_3_clicked()
{
    quint8 type=4;
    emit agree(type,getUserId());
    emit clear();
}

void userInfo::on_pushButton_2_clicked()
{
    emit clear();
}

void userInfo::on_pushButton_4_clicked()
{
    emit clear();
}
