#include "userpanel.h"
#include "ui_userpanel.h"
#include <QDebug>
#include <QMessageBox>

userPanel::userPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::userPanel)
{
    ui->setupUi(this);
    ui->id->setReadOnly(true);
    ui->pushButton->setVisible(false);
    setUpVisible(false);
}

userPanel::~userPanel()
{
    delete ui;
}

void userPanel::setUser(QByteArray &user)
{
    QDataStream in(&user,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    in>>m_id>>m_name>>m_sex>>m_age>>m_area>>m_photo>>m_motto;
    ui->id->setText(m_id);
    ui->name->setText(m_name);
    if(!m_sex)
        ui->sex->setCurrentIndex(0);
    else
        ui->sex->setCurrentIndex(1);
    ui->age->setValue(m_age);
    ui->area->setText(m_area);
    QPixmap map;
    map.loadFromData(m_photo);
    ui->photo->setPixmap(map);
    ui->motto->setPlainText(m_motto);
}

void userPanel::setmodify(bool n)
{
    ui->pushButton->setVisible(n);
}

void userPanel::setUpVisible(bool b)
{
    ui->name->setReadOnly(!b);
    ui->sex->setDisabled(!b);
    ui->area->setReadOnly(!b);
    ui->age->setReadOnly(!b);
    ui->motto->setReadOnly(!b);
    ui->pushButton_2->setVisible(b);
}

void userPanel::setUp(bool b)
{
    if(b)
    {
        QMessageBox::information(this,tr("修改个人信息"),tr("修改成功"));
        m_name=ui->name->text();
        if(ui->sex->currentIndex()==0)
            m_sex=false;
        else
            m_sex=true;
        m_age=ui->age->value();
        m_area=ui->area->text();
        m_motto=ui->motto->toPlainText();
    }
    else
    {
        QMessageBox::information(this,tr("修改个人信息"),tr("修改失败"));
    }
}

void userPanel::on_pushButton_clicked()
{
    setUpVisible(true);
    ui->pushButton->setEnabled(false);
    if(m_avatar==nullptr)
    {
        m_avatar=new QMavatar;
        connect(m_avatar,&QMavatar::iconinfo,
                this,&userPanel::setPhoto);
    }
}

void userPanel::on_pushButton_2_clicked()
{
    setUpVisible(false);
    ui->pushButton->setEnabled(true);
    QString name=ui->name->text();
    bool sex;
    if(ui->sex->currentIndex()==0)
        sex=false;
    else
        sex=true;
    int age=ui->age->value();
    QString area=ui->area->text();
    QString motto=ui->motto->toPlainText();
    QByteArray info;
    QDataStream out(&info,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out<<m_id<<name<<sex<<age<<area<<m_photo<<motto;
    quint8 type=23;
    emit userInfo(type,info);
    delete m_avatar;
    m_avatar=nullptr;
}

void userPanel::on_photo_clicket()
{
    if(m_avatar!=nullptr)
        m_avatar->show();
}

void userPanel::setPhoto(QByteArray photo)
{
    m_photo=photo;
    QPixmap map;
    map.loadFromData(m_photo);
    ui->photo->setPixmap(map);
}
