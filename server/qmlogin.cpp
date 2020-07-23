#include "qmlogin.h"
#include "ui_qmlogin.h"

#include <QSqlRecord>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QDebug>

QMLogin::QMLogin(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::QMLogin)
{
    ui->setupUi(this);
    db=QSqlDatabase::addDatabase("QSQLITE","login");
    db.setDatabaseName("da.db");
    m_server=new QMServer(this);
}

QMLogin::~QMLogin()
{
    delete ui;
}

QString QMLogin::encrypt(QString &str)
{
    QByteArray data;
    data.append(str);
    QCryptographicHash hash(QCryptographicHash::Sha3_512);
    hash.addData(data);
    QByteArray da=hash.result();
    QString n=da.toHex();
    return n;
}

bool QMLogin::login(QString &id, QString &pass)
{
    pass=encrypt(pass);
    QSqlRecord re;
    if(!db.open())
        QMessageBox::information(this,tr("提示"),tr("数据库打开失败！"));
    QSqlQuery qu(db);
    qu.prepare("select * from m_root where root=:id");
    qu.bindValue(":id",id);
    qu.exec();
    if(!qu.isActive()){
        return false;
    }
    if(qu.next()){
        re=qu.record();
    }
    db.close();
    QString password=re.value(2).toString();
    if(password==pass){
        return true;
    }
    else if(r==id){//判断内置账号
     if(p==pass)
         return true;
    }
    return false;
}


void QMLogin::on_pushButton_clicked()
{
    QString id=ui->lineEdit->text();

    if(id.isEmpty())
    {
        ui->label_3->setText(tr("请输入账号"));
        return;
    }
    QString pass=ui->lineEdit_2->text();
    if(pass.isEmpty())
    {
        ui->label_3->setText(tr("请输入密码"));
        return;
    }
    bool isLogin=login(id,pass);
    if(isLogin)
    {
        m_server->show();
        this->setVisible(false);
    }
    else
    {
        ui->label_3->setText("账号或密码错误");
    }
}

void QMLogin::on_lineEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    ui->label_3->setText("");
}

void QMLogin::on_lineEdit_2_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    ui->label_3->setText("");
}
