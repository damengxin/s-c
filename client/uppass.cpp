#include "uppass.h"
#include "ui_uppass.h"
#include <QCryptographicHash>

upPass::upPass(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::upPass)
{
    ui->setupUi(this);
}

upPass::~upPass()
{
    delete ui;
}

QString upPass::encrypt(QString &str)
{
    QByteArray data;
    data.append(str);
    QCryptographicHash hash(QCryptographicHash::Sha3_512);
    hash.addData(data);
    QByteArray da=hash.result();
    QString n=da.toHex();
    return n;
}

void upPass::on_pushButton_clicked()
{
    QString pass1=ui->lineEdit->text();
    QString pass2=ui->lineEdit_2->text();
    if(pass1!=pass2)
    {
        ui->label_3->setStyleSheet("color: rgb(255, 0, 0);");
        ui->label_3->setText(tr("输入的密码不一致"));
        return;
    }
    ui->label_3->setText("");
    pass2=encrypt(pass2);
    emit pass(pass2);
}
