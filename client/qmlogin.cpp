#include "qmlogin.h"
#include "ui_qmlogin.h"
#include <QCryptographicHash>
#include <QRegExpValidator>

void QMLogin::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    if(thread!=nullptr)
        if(thread->isRunning())
            thread->quit();
}

QMLogin::QMLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QMLogin)
{
    ui->setupUi(this);
    QRegExp re("^(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|[1-9])\\."
               "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)\\."
               "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)\\."
               "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)$");
    QValidator *validator=new QRegExpValidator(re,this);
    ui->ip->setValidator(validator);
    ui->groupBox_2->setVisible(false);
    ui->pushButton_7->setEnabled(false);
    isSocket=false;
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

void QMLogin::userLoginCheck(QByteArray data)
{//21
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    bool b;
    QByteArray user;
    in>>b>>user;
    if(b)
    {
        QByteArray info;
        QDataStream out(&info,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_13);
        out<<userId;
        quint8 type=26;
        emit send(type,info);
        m_client->setUser(user);
    }
    else
    {
        ui->label_3->setStyleSheet(
                    "color: rgb(255, 85, 0);");
        ui->label_3->setText(tr("账号或密码不正确"));
    }
}

void QMLogin::newUserId(QByteArray data)
{//22
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QString id;
    in>>id;
    m_newUser->setId(id);
}

void QMLogin::userFrined(QByteArray data)
{//26
    m_client->userFrined(data);

    m_client->show();
    this->setVisible(false);
}

void QMLogin::read_Data(QByteArray data)
{//信息分类处理
    quint8 index;
    int size;
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QByteArray da,err;
    in>>index>>size>>da;
    err=data;
    qDebug()<<"Login "<<index;
    switch (index)
    {
    case 21:userLoginCheck(da);data.clear();return;
    case 22:newUserId(da);data.clear();return;
    case 26:userFrined(da);data.clear();return;
    case 1:emit tcpData(data);data.clear();return;
    case 3:emit tcpData(data);data.clear();return;
    case 5:emit tcpData(data);data.clear();return;
    case 23:emit tcpData(data);data.clear();return;
    case 24:emit tcpData(data);data.clear();return;
    case 25:emit tcpData(data);data.clear();return;
    case 27:emit tcpData(data);data.clear();return;
    case 28:m_forget->setResult(da);data.clear();return;
    default:error(err);data.clear();return;
    }

}

void QMLogin::error(QByteArray data)
{

}

void QMLogin::hostConnect()
{
    isSocket=true;
    ui->label_5->setStyleSheet(
                "background:transparent;");
    ui->label_5->setText("");
}

void QMLogin::dis()
{
    if(thread->isRunning())
        thread->quit();
}

void QMLogin::udp_send(quint8 type,QByteArray info)
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out<<type<<info;
    m_udp->writeDatagram(data,data.size(),m_address,m_port+1);
}

void QMLogin::udp_read()
{
    if(m_udp->hasPendingDatagrams())
    {
        QByteArray data;
        data.resize(m_udp->pendingDatagramSize());
        m_udp->readDatagram(data.data(),data.size());
        quint8 type;
        QByteArray da;
        QDataStream in(&data,QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_13);
        in>>type>>da;
        qDebug()<<"udp read logiin"<<type;
        switch (type) {
        case 11:emit read_udpInfo(da);return;
        }

    }
}

void QMLogin::renewLogin()
{
    this->show();
    m_client->setVisible(false);
    on_pushButton_7_clicked();
}

void QMLogin::on_pushButton_5_clicked()
{
    bool b=ui->groupBox_2->isVisible();
    ui->groupBox_2->setVisible(!b);
    if(!b)
        ui->pushButton_6->setFocus();
}

void QMLogin::on_pushButton_2_clicked()
{
    if(!isSocket)
    {
        ui->label_5->setStyleSheet(
                    "background-color:rgb(255,255,127);");
        ui->label_5->setText(tr("网络未连接"));
        return;
    }
    QString id=ui->lineEdit->text();
    QString pass=ui->lineEdit_2->text();
    if(id.isEmpty())
    {
        ui->label_3->setStyleSheet(
                    "color: rgb(255, 85, 0);");
        ui->label_3->setText(tr("请输入账号"));
        return;
    }
    userId=id;
    if(pass.isEmpty())
    {
        ui->label_3->setStyleSheet(
                    "color: rgb(255, 85, 0);");
        ui->label_3->setText(tr("请输入密码"));
        return;
    }
    QByteArray info;
    QDataStream out(&info,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    pass=encrypt(pass);
    out<<id<<pass;
    quint8 type=21;
    emit send(type,info);
}

void QMLogin::on_lineEdit_textChanged(const QString &arg1)
{
    ui->label_3->setText("");
}

void QMLogin::on_lineEdit_2_textChanged(const QString &arg1)
{
    ui->label_3->setText("");
}

void QMLogin::on_pushButton_6_clicked()
{
    m_ip=ui->ip->text();
    QString port=ui->port->text();
    if(m_ip.isEmpty())
    {
        ui->label_4->setStyleSheet(
                    "color: rgb(255, 85, 0);");
        ui->label_4->setText(tr("请输入IP地址"));
        return;
    }
    if(port.isEmpty())
    {
        ui->label_4->setStyleSheet(
                    "color: rgb(255, 85, 0);");
        ui->label_4->setText(tr("请输入端口"));
        return;
    }
    m_port=port.toUInt();
    m_address.setAddress(m_ip);

    m_client=new QMUser;
    m_client->setWindowTitle(tr(" "));
    m_client->setIpPort(m_ip,m_port);
    m_udp=new QUdpSocket(this);
    m_udp->abort();
    m_udp->bind(QHostAddress::AnyIPv4,m_port+2);
    thread=new QThread(this);
    thread->start();
    m_socket=new TcpSocket;
    m_socket->moveToThread(thread);

    connect(m_socket,&TcpSocket::readyRead,
            m_socket,&TcpSocket::receive_Data);
    connect(m_socket,&TcpSocket::disconnected,
            m_socket,&TcpSocket::disConnect);
    connect(m_socket,&TcpSocket::read_Data,
            this,&QMLogin::read_Data);
    connect(m_socket,&TcpSocket::clientError,
            this,&QMLogin::error);
    connect(m_socket,&TcpSocket::connected,
            this,&QMLogin::hostConnect);
    connect(this,&QMLogin::connectHost,
            m_socket,&TcpSocket::connectHost);
    connect(this,&QMLogin::send,
            m_socket,&TcpSocket::send_Data);
    connect(this,&QMLogin::disConnect,
            m_socket,&TcpSocket::disConnect);

    connect(m_client,&QMUser::send_udp,
            this,&QMLogin::udp_send);
    connect(m_udp,&QUdpSocket::readyRead,
            this,&QMLogin::udp_read);
    connect(this,&QMLogin::read_udpInfo,
            m_client,&QMUser::udp_readInfo);

    connect(m_client,&QMUser::dis,
            this,&QMLogin::dis);
    connect(m_client,&QMUser::send_tcp,
            this,&QMLogin::send);
    connect(m_client,&QMUser::renewLogin,
            this,&QMLogin::renewLogin);
    connect(this,&QMLogin::tcpData,
            m_client,&QMUser::user_data);

    emit connectHost(m_ip,m_port);
    ui->pushButton_6->setEnabled(false);
    ui->pushButton_7->setEnabled(true);
    ui->pushButton_2->setFocus();
}

void QMLogin::on_ip_textChanged(const QString &arg1)
{
    ui->label_4->setText("");
}

void QMLogin::on_port_textChanged(const QString &arg1)
{
    ui->label_4->setText("");
}

void QMLogin::on_pushButton_7_clicked()
{
    emit disConnect();
    delete m_client;
    delete m_udp;
    thread->quit();
    thread=nullptr;
    m_socket=nullptr;
    m_udp=nullptr;
    m_client=nullptr;
    isSocket=false;
    ui->pushButton_6->setEnabled(true);
    ui->pushButton_7->setEnabled(false);
    ui->pushButton_6->setFocus();
}

void QMLogin::on_pushButton_4_clicked()
{
    QFont font=ui->pushButton_4->font();
    font.setUnderline(false);
    ui->pushButton_4->setFont(font);
    if(!isSocket)
    {
        ui->label_5->setStyleSheet(
                    "background-color:rgb(255,255,127);");
        ui->label_5->setText(tr("网络未连接"));
        return;
    }
    if(m_newUser==nullptr)
    {
        m_newUser=new newUser;
        m_newUser->setWindowTitle(tr("账号申请"));
        connect(m_newUser,&newUser::new_User,
                this,&QMLogin::send);
    }
    m_newUser->show();
}

void QMLogin::on_pushButton_4_pressed()
{
    QFont font=ui->pushButton_4->font();
    font.setUnderline(true);
    ui->pushButton_4->setFont(font);
}

void QMLogin::on_pushButton_3_clicked()
{
    QFont font=ui->pushButton_3->font();
    font.setUnderline(false);
    ui->pushButton_3->setFont(font);
    if(!isSocket)
    {
        ui->label_5->setStyleSheet(
                    "background-color:rgb(255,255,127);");
        ui->label_5->setText(tr("网络未连接"));
        return;
    }
    if(m_forget==nullptr)
    {
        m_forget=new forget;
        m_forget->setWindowTitle(tr("忘记密码"));
        connect(m_forget,&forget::for_get,
                this,&QMLogin::send);
    }
    m_forget->setVisi(true);
    m_forget->show();
}

void QMLogin::on_pushButton_3_pressed()
{
    QFont font=ui->pushButton_3->font();
    font.setUnderline(true);
    ui->pushButton_3->setFont(font);
}
