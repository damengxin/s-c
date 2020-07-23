#include "userpublic.h"
#include "ui_userpublic.h"
#include <QMessageBox>

userPublic::userPublic(QString id,QString name,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::userPublic)
{
    ui->setupUi(this);
    m_id=id;
    m_name=name;
    isOpen=false;
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
}

userPublic::~userPublic()
{
    delete ui;
}

void userPublic::iniSocket()
{
    m_publicSocket=new QUdpSocket;
    m_publicSocket->bind(QHostAddress::AnyIPv4,m_publicPort,QUdpSocket::ShareAddress);
    m_publicAddress.setAddress("239.255.255.255");
    connect(m_publicSocket,&QUdpSocket::readyRead,
            this,&userPublic::readPublic);
    //只在局域网传播
    m_publicSocket->setSocketOption(QAbstractSocket::MulticastTtlOption,1);
    //组播回环
    m_publicSocket->setSocketOption(QAbstractSocket::MulticastLoopbackOption,false);
    //加入多播组
    m_publicSocket->joinMulticastGroup(m_publicAddress);
}

void userPublic::setIpPort(QString ip, quint16 port)
{
    m_ip=ip;
    m_publicPort=port+3;
    m_address.setAddress(m_ip);
}

void userPublic::readPublic()
{
    if(m_publicSocket->hasPendingDatagrams())
    {
        QByteArray data,da;
        data.resize(m_publicSocket->pendingDatagramSize());
        m_publicSocket->readDatagram(data.data(),data.size());
        QDataStream in(&data,QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_13);
        quint8 type;
        in>>type>>da;
        qDebug()<<"public read"<<type;
        switch (type)
        {
        case 41:userAddPublic(da);return;
        case 42:userGone(da);return;
        case 43:msgPublic(da);return;
        }
    }
}

void userPublic::sendPublic(quint8 type, QByteArray info)
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out<<type<<info;
    qDebug()<<type<<
              m_publicAddress.toString()<<m_publicPort;
    m_publicSocket->writeDatagram(data,m_publicAddress,m_publicPort);
}

void userPublic::userAddPublic(QByteArray info)
{
    QDataStream in(&info,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QString id,name;
    in>>id>>name;
    listId.append(id);
    listName.append(name);
    ui->textEdit->setAlignment(Qt::AlignRight);
    ui->textEdit->append(tr("%1用户%2加入聊天室\n")
                         .arg(times())
                         .arg(name));
    QListWidgetItem *item=new QListWidgetItem;
    item->setText(tr("%1").arg(name));
    item->setData(2000,id);
    ui->listWidget->addItem(item);
}

void userPublic::userGone(QByteArray info)
{
    QDataStream in(&info,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QString id;
    in>>id;
    for(int i=0;i<listId.count();i++)
    {
        QString id=listId.at(i);
        if(id==id)
        {
            QListWidgetItem *item=ui->listWidget->item(i);
            QString itemid=item->data(2000).toString();
            item=ui->listWidget->takeItem(i);
            delete item;
            listId.takeAt(i);
            ui->textEdit->setAlignment(Qt::AlignRight);
            ui->textEdit->append(tr("%1用户%2退出聊天室\n")
                                 .arg(times())
                                 .arg(listName.takeAt(i)));
            return;
        }
    }
}

void userPublic::msgPublic(QByteArray info)
{
    QDataStream in(&info,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QString sendid,time,msg;
    in>>sendid>>time>>msg;
    for(int i=0;i<listId.count();i++)
    {
        QString id=listId.at(i);
        if(sendid==id)
        {
            ui->textEdit->setAlignment(Qt::AlignLeft);
            ui->textEdit->append(tr("%1 %2:\n%3\n")
                         .arg(listName.at(i))
                         .arg(times())
                         .arg(msg));
            return;
        }
    }
}

void userPublic::up(QString naem)
{
    m_name=naem;
}

void userPublic::isPublicOpen(QByteArray info)
{
    QDataStream in(&info,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    in>>isOpen;
    if(!isOpen)
    {
        ui->pushButton->setEnabled(false);
        ui->pushButton_2->setEnabled(false);
        isOpen=false;
        QMessageBox::information(this,tr("提示"),
                                 tr("聊天室未开启，请联系管理员"));
        return;
    }
    else
    {
        iniSocket();
        in>>listId>>listName;
        for(int i=0;i<listId.count();i++)
        {
            QListWidgetItem *item=new QListWidgetItem;
            item->setText(tr("%1").arg(listName.at(i)));
            item->setData(2000,listId.at(i));
            ui->listWidget->addItem(item);
        }
        ui->pushButton->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
        ui->lineEdit->setText(tr("%1").arg(listId.count()));

        isOpen=true;
        QByteArray info;
        QDataStream out(&info,QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_13);
        quint8 type=41;
        out<<m_id<<m_name;
        sendPublic(type,info);
        //因为设置了信息回环，收不到自己的信息
        //用户列表添加自己
        QListWidgetItem *item=new QListWidgetItem;
        item->setText(tr("%1").arg(m_name));
        item->setData(2000,m_id);
        ui->listWidget->addItem(item);
    }
}

void userPublic::emojiMsg(QString s)
{
    QString text=ui->textEdit_2->toPlainText();
    ui->textEdit_2->setText(text+s);
}

void userPublic::visible()
{
    ui->pushButton_3->setIcon(QIcon(":/icon/bq.png"));
}

void userPublic::on_pushButton_clicked()
{
    QString msg=ui->textEdit_2->toPlainText();
    if(msg.isEmpty())
        return;
    msg=msg.trimmed();
    ui->textEdit_2->clear();
    ui->textEdit->setAlignment(Qt::AlignRight);
    ui->textEdit->append(tr("%1 %2:\n%3\n")
                         .arg(m_name)
                         .arg(times())
                         .arg(msg));
    QByteArray info;
    QDataStream out(&info,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out<<m_id<<times()<<msg;
    quint8 type=43;
    sendPublic(type,info);
}

void userPublic::on_pushButton_2_clicked()
{//退出聊天室
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->listWidget->clear();
    ui->lineEdit->setText(tr("0"));

    QByteArray info;
    QDataStream out(&info,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    quint8 type=42;
    out<<m_id;

    sendPublic(type,info);
    m_publicSocket->leaveMulticastGroup(m_publicAddress);
    delete m_publicSocket;
    m_publicSocket=nullptr;
    emit clear();
    isOpen=false;
}

void userPublic::on_pushButton_3_clicked()
{
    if(m_emoji==nullptr)
    {
        m_emoji=new QMEmoji(this);
        m_emoji->setVisible(false);

        connect(m_emoji,SIGNAL(emoji(QString)),
                this,SLOT(emojiMsg(QString)));
        connect(m_emoji,SIGNAL(visible()),
                this,SLOT(visible()));
    }
    m_emoji->setGeometry(11,260,420,140);
    m_emoji->setTableVisible();
    ui->pushButton_3->setIcon(QIcon(":/icon/bq1.png"));
    m_emoji->show();
}
