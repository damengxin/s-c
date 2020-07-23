#include "newuser.h"
#include "ui_newuser.h"
#include <QCryptographicHash>
#include <QMessageBox>

newUser::newUser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::newUser)
{
    ui->setupUi(this);
    QString filename=QString(":/icon/tx1.jpg");
    QFile fi(filename);
    fi.open(QIODevice::ReadOnly);
    m_photo=fi.readAll();
    fi.close();
    QPixmap map;
    map.loadFromData(m_photo);
    ui->photo->setPixmap(map);
    ui->name->setMaxLength(20);
    ui->area->setMaxLength(20);
    ui->pass_1->setMaxLength(15);
    ui->pass_2->setMaxLength(15);
    ui->m_1->setMaxLength(16);
    ui->m_2->setMaxLength(16);
}

newUser::~newUser()
{
    delete ui;
}

QString newUser::encrypt(QString &str)
{
    QByteArray data;
    data.append(str);
    QCryptographicHash hash(QCryptographicHash::Sha3_512);
    hash.addData(data);
    QByteArray da=hash.result();
    QString n=da.toHex();
    return n;
}

void newUser::setId(QString id)
{
    QMessageBox::information(this,tr("申请账号"),id);
}

void newUser::setPhoto(QByteArray photo)
{
    m_photo=photo;
    QPixmap map;
    map.loadFromData(m_photo);
    ui->photo->setPixmap(map);
}

void newUser::on_pushButton_clicked()
{
    QString pass1=ui->pass_1->text();
    QString pass2=ui->pass_2->text();
    if(pass1!=pass2)
    {
        ui->label_14->setStyleSheet("color: rgb(255, 0, 0);");
        ui->label_14->setText(tr("两次输入的密码不一致！"));
        return;
    }
    ui->label_14->setText("");
    QString name=ui->name->text();
    bool sex;
    if(ui->sex->currentIndex()==0)
        sex=false;
    else
        sex=true;
    int age=ui->age->value();
    QString area=ui->area->text();
    QString motto=ui->motto->toPlainText();
    QString m1=ui->m_1->text();
    QString m2=ui->m_2->text();
    if(name.isEmpty()||pass1.isEmpty()||pass2.isEmpty()
      ||m1.isEmpty()||m2.isEmpty())
    {
        ui->label_14->setStyleSheet("color: rgb(255, 0, 0);");
        ui->label_14->setText(tr("请输入必填项"));
        return;
    }
    pass1=encrypt(pass1);
    m1=encrypt(m1);
    m2=encrypt(m2);
    QByteArray info;
    QDataStream out(&info,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out<<name<<sex<<age<<area<<pass1<<m_photo<<motto<<m1<<m2;
    quint8 type=22;
    emit new_User(type,info);
}

void newUser::on_pass_2_editingFinished()
{
    QString pass1=ui->pass_1->text();
    QString pass2=ui->pass_2->text();
    if(pass1!=pass2)
    {
        ui->label_14->setStyleSheet("color: rgb(255, 0, 0);");
        ui->label_14->setText(tr("两次输入的密码不一致！"));
        return;
    }
    ui->label_14->setText("");
}

void newUser::on_photo_clicket()
{
    if(m_avatar==nullptr)
    {
        m_avatar=new QMavatar(this);
        connect(m_avatar,&QMavatar::iconinfo,
                this,&newUser::setPhoto);
    }
    m_avatar->show();
}
