#include "forget.h"
#include "ui_forget.h"
#include <QCryptographicHash>
#include <QMessageBox>

forget::forget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::forget)
{
    ui->setupUi(this);
    ui->pass_1->setMaxLength(15);
    ui->pass_2->setMaxLength(15);
    ui->m_1->setMaxLength(16);
    ui->m_2->setMaxLength(16);
}

forget::~forget()
{
    delete ui;
}

QString forget::encrypt(QString &str)
{
    QByteArray data;
    data.append(str);
    QCryptographicHash hash(QCryptographicHash::Sha3_512);
    hash.addData(data);
    QByteArray da=hash.result();
    QString n=da.toHex();
    return n;
}

void forget::setResult(QByteArray info)
{
    QDataStream in(&info,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    bool b;
    in>>b;
    if(!b)
    {
        QString s;
        in>>s;
        QMessageBox::warning(this,tr("信息"),tr("%1").arg(s));
    }
    else
    {
        QMessageBox::information(this,tr("信息"),tr("修改成功"));
        ui->id->clear();
        ui->m_1->clear();
        ui->m_2->clear();
        ui->pass_1->clear();
        ui->pass_2->clear();
        setVisi(false);
    }
}

void forget::setVisi(bool b)
{
    ui->groupBox->setVisible(b);
    ui->pushButton->setVisible(b);
}

void forget::on_pushButton_clicked()
{
    QString id=ui->id->text();
    QString m_1=ui->m_1->text();
    QString m_2=ui->m_2->text();
    QString pass_1=ui->pass_1->text();
    QString pass_2=ui->pass_2->text();
    if(id.isEmpty()||m_1.isEmpty()||m_2.isEmpty()
            ||pass_1.isEmpty()||pass_2.isEmpty())
    {
        ui->label_6->setText(tr("请完善信息"));
        return;
    }
    if(pass_1!=pass_2)
    {
        ui->label_6->setText(tr("两次输入密码不一致"));
        return;
    }
    ui->label_6->setText("");
    pass_1=encrypt(pass_2);
    m_1=encrypt(m_1);
    m_2=encrypt(m_2);
    QByteArray info;
    QDataStream out(&info,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out<<id<<m_1<<m_2<<pass_1;
    quint8 type=28;
    emit for_get(type,info);
}
