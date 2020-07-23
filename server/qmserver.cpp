#include "qmserver.h"
#include "ui_qmserver.h"

QMServer::QMServer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QMServer)
{
    ui->setupUi(this);
    ini();
}

QMServer::~QMServer()
{
    delete ui;
}

void QMServer::ini()
{
    QRegExp re("^(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|[1-9])\\."
               "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)\\."
               "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)\\."
               "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)$");
    QValidator *validator=new QRegExpValidator(re,this);
    ui->ip->setValidator(validator);
    ui->ip->setReadOnly(false);
    ui->port->setReadOnly(false);
    ui->listWidget->setViewMode(QListView::ListMode);
    ui->listWidget->setResizeMode(QListView::Adjust);
    QString str;
    str="QHeaderView::section{"
        "background:rgb(0,210,255);"//表头背景色
        "color: rgb(0, 0, 0)};";//表头字体颜色
    ui->tableWidget->setColumnWidth(0,165);
    ui->tableWidget->setColumnWidth(1,136);
    ui->tableWidget->setColumnWidth(2,136);
    ui->tableWidget->setColumnWidth(3,165);
    ui->tableWidget->horizontalHeader()->setStyleSheet(str);
    //列宽不可改变
    ui->tableWidget->horizontalHeader()->setDisabled(true);
    ui->tab->setStyleSheet(str);
    //工具栏字体颜色
    ui->toolBar->setStyleSheet("color: rgb(255,255,255)");
    ui->start->setEnabled(true);
    ui->action->setEnabled(false);
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    m_data=new QMData("QSQLITE",this);
    connect(m_data,&QMData::dataError,
            this,&QMServer::dataError);

    m_server=new QMTcpServer;
    m_udpSocket=new QUdpSocket(this);
    m_publicSocket=new QUdpSocket(this);
}

void QMServer::on_start_triggered()
{
    //Tcp服务器
    m_HostAddress.setAddress(ui->ip->text());
    m_ServerPort=ui->port->text().toUShort();
    if(!m_server->listen(m_HostAddress,m_ServerPort))
    {
        QMessageBox::critical(this,tr("错误"),tr("错误：%1")
                              .arg(m_server->errorString()));
        return;
    }
    connect(m_server,&QMTcpServer::newConnection,
            this,&QMServer::newConnect);

    m_udpSocket=new QUdpSocket(this);
    m_udpSocket->bind(m_HostAddress,m_ServerPort+1);
    connect(m_udpSocket,&QUdpSocket::readyRead,
            this,&QMServer::udpRead);
    ui->start->setEnabled(false);
    ui->action->setEnabled(true);
    ui->ip->setEnabled(false);
    ui->port->setEnabled(false);
    ui->plainTextEdit->appendPlainText(tr("%1:服务器开启").arg(time()));
    ui->lineEdit->setText(tr("服务器监听中"));
}

void QMServer::on_action_triggered()
{
    if(m_server->isListening())//正在监听
    {
        m_server->close();
        m_udpSocket->abort();
        ui->plainTextEdit->appendPlainText(tr("%1:服务器关闭 ").arg(time()));
        ui->start->setEnabled(true);
        ui->action->setEnabled(false);
        on_pushButton_2_clicked();
        ui->lineEdit->setText(tr("停止监听 "));
        ui->ip->setReadOnly(false);
        ui->port->setReadOnly(false);
        ui->ip->setEnabled(true);
        ui->port->setEnabled(true);
        ui->lineEdit->setText(tr("无监听"));
    }
}

void QMServer::udpRead()
{
    if(m_udpSocket->hasPendingDatagrams())
    {
        QByteArray data,da;
        data.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(data.data(),data.size());
        QDataStream in(&data,QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_13);
        quint8 type;
        in>>type>>da;
        qDebug()<<"udp read";
        switch (type)
        {
        case 11:userPrivate(da);return;
        }
    }
}

void QMServer::udpSend(QByteArray info,QString ip,quint8 type)
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out<<type<<info;
    QHostAddress address(ip);
    qDebug()<<"udp send"<<ip<<m_ServerPort+2;
    m_udpSocket->writeDatagram(data,data.size(),
                               address,m_ServerPort+2);
}

void QMServer::readPublic()
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

void QMServer::sendPublic(quint8 type,QByteArray info)
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out<<type<<info;
    qDebug()<<type
      <<m_publicAddress.toString()<<m_publicPort;
    m_publicSocket->writeDatagram(data,m_publicAddress,
                                  m_publicPort);
}

void QMServer::getPublicUser()
{
    TcpSocket *socket=qobject_cast<TcpSocket*>(sender());
    QByteArray info;
    QDataStream out(&info,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out<<isOpen<<listId<<listName;
    quint8 type=27;
    emit send_Tcp(type,info);
}

void QMServer::userAddPublic(QByteArray info)
{
    QDataStream in(&info,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QString id,name;
    in>>id>>name;
    listId.append(id);
    listName.append(name);
    ui->textEdit->setAlignment(Qt::AlignLeft);
    ui->textEdit->append(tr("%1用户%2加入聊天室\n")
                         .arg(times())
                         .arg(name));
    QListWidgetItem *item=new QListWidgetItem;
    item->setText(tr("%1").arg(name));
    item->setData(2000,id);
    ui->listWidget->addItem(item);
}

void QMServer::userGone(QByteArray info)
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
            ui->textEdit->setAlignment(Qt::AlignLeft);
            ui->textEdit->append(tr("%1用户%2退出聊天室\n")
                                 .arg(time())
                                 .arg(listName.takeAt(i)));
            return;
        }
    }
}

void QMServer::msgPublic(QByteArray info)
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

void QMServer::newConnect()
{
    TcpSocket*socket=m_server->nextPendingConnection();
    connect(socket,&TcpSocket::dis,
            this,&QMServer::userDisconnect);
    connect(socket,&TcpSocket::dataError,
            this,&QMServer::dataError);
    connect(socket,&TcpSocket::client,
            this,&QMServer::clientError);
    connect(socket,&TcpSocket::userErrorInfo,
            this,&QMServer::userErrorInfo);
    connect(socket,&TcpSocket::userLoginInfo,
            this,&QMServer::userLoginInfo);
    connect(socket,&TcpSocket::frinedAdd,
            this,&QMServer::frinedAdd);
    connect(socket,&TcpSocket::upFrinedList,
            this,&QMServer::upFrinedList);
    connect(socket,&TcpSocket::publicUser,
            this,&QMServer::getPublicUser);
    connect(this,&QMServer::send_Tcp,
            socket,&TcpSocket::send_Data);
    ui->plainTextEdit->appendPlainText(
                tr("%1有新连接").arg(time()));
}

void QMServer::userLoginInfo(QString id)
{
    QString ip;
    quint16 port;
    TcpSocket *socket=qobject_cast<TcpSocket*>(sender());
    m_SocketS.insert(socket,id);
    ip=socket->peerAddress().toString();
    port=socket->peerPort();
    m_data->userConnect(id,ip);//数据库保存
    int row=ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);
    QTableWidgetItem *iditem=new QTableWidgetItem;
    iditem->setText(id);
    ui->tableWidget->setItem(row,0,iditem);
    //地址
    QTableWidgetItem *addressitem=new QTableWidgetItem;
    addressitem->setText(ip);
    ui->tableWidget->setItem(row,1,addressitem);
    //端口
    QTableWidgetItem *portitem=new QTableWidgetItem;
    portitem->setText(tr("%1").arg(port));
    ui->tableWidget->setItem(row,2,portitem);
    //登录时间
    QTableWidgetItem *timeitem=new QTableWidgetItem;
    timeitem->setText(time());
    ui->tableWidget->setItem(row,3,timeitem);
    ui->tableWidget->setCurrentItem(timeitem);
    ui->plainTextEdit->appendPlainText(tr("%1用户:%2登录")
                                       .arg(time())
                                       .arg(id));
}

void QMServer::userErrorInfo(QByteArray data)
{
    TcpSocket *socket=qobject_cast<TcpSocket*>(sender());
    QString id=m_SocketS.value(socket);
    ui->plainTextEdit->appendPlainText(
                tr("%1用户%2发送无法识别的信息:\n")
                .arg(time())
                .arg(id)
                +data);
}

void QMServer::dataError(QString error)
{
    ui->plainTextEdit->appendPlainText(time()+error);
}

void QMServer::clientError(QString id,QByteArray error)
{
    ui->plainTextEdit->appendPlainText(tr("%1客户端产生错误:\n")
                                    .arg(time()+id)
                                    +error);
}

void QMServer::userDisconnect(bool b,QThread *thread)
{
    if(!b)
    {
        TcpSocket *socket=qobject_cast<TcpSocket*>(sender());
        QString id=m_SocketS.take(socket);
        m_data->userDisconnect(id);
        for (int i=0;i<ui->tableWidget->rowCount();i++)
        {
            QTableWidgetItem *item=new QTableWidgetItem;
            item=ui->tableWidget->item(i,0);
            if(item->text()==id)
            {
                ui->tableWidget->removeRow(i);
                ui->plainTextEdit->appendPlainText(
                            tr("%1:用户%2下线")
                            .arg(time())
                            .arg(id));
                delete socket;
                delete thread;
                return;
            }
        }
        ui->plainTextEdit->appendPlainText(
                    tr("%1有连接断开")
                    .arg(time()));
        delete socket;
        delete thread;
    }
}

QString QMServer::getUserIp(QString &userId)
{
    QSqlRecord re=m_data->userSelect(userId);
    if(re.isEmpty())
    {
        return "";
    }
    QString ip=re.value("ip").toString();
    return ip;
}

QByteArray QMServer::getUserInfo(QString &userId)
{//qDebug()<<"server type2.1.1";
    QByteArray data;
    QSqlRecord re=m_data->userSelect(userId);
    if(re.isEmpty())
    {
        return data;
    }
    QString id=re.value("user_id").toString();
    QString name=re.value("user_name").toString();
    bool sex=re.value("user_sex").toBool();
    int age=re.value("user_age").toInt();
    QString area=re.value("user_area").toString();
    QByteArray photo=re.value("user_photo").toByteArray();
    QString motto=re.value("user_motto").toString();
    QDataStream out(&data,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out<<id<<name<<sex<<age<<area<<photo<<motto;
    return data;
}

void QMServer::userPrivate(QByteArray data)
{//11
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_13);
    QString receiveid,sendid,msg,time,ip;
    time=times();
    in>>receiveid>>sendid>>msg;
    QByteArray info;
    QDataStream out(&info,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out<<sendid<<time<<msg;
    ip=getUserIp(receiveid);
    if(ip.isEmpty())
    {   quint8 type=1;
        m_data->saveInfo(receiveid,type,info);
    }
    else
    {
        qDebug()<<"udp send 11"<<ip;
        quint8 type=11;
        udpSend(info,ip,type);
    }
}

void QMServer::emojiMsg(QString s)
{
    QString text=ui->textEdit_2->toPlainText();
    ui->textEdit_2->setText(text+s);
}

void QMServer::visible()
{
    ui->pushButton_4->setIcon(QIcon(":/icon/bq.png"));
}

void QMServer::frinedAdd(QString frinedid, QString userid)
{//3
    qDebug()<<"frinedAdd  4.3"<<frinedid<<userid;
    QByteArray data=getUserInfo(userid);
    QString ip=getUserIp(frinedid);
    qDebug()<<"frinedAdd  1"<<ip;
    if(ip.isEmpty())//用户不在线
    {qDebug()<<"frinedAdd  2";
        m_data->saveInfo(frinedid,3,data);
        return;
    }
    else
    {//用户在线
        qDebug()<<"frinedAdd  3";
        TcpSocket *socket=m_SocketS.key(frinedid);
        connect(this,&QMServer::frined,
                socket,&TcpSocket::frined);
        emit frined(data);
        disconnect(this,&QMServer::frined,
                socket,&TcpSocket::frined);
    }
}

void QMServer::upFrinedList(QString userId, QString frinedId)
{
    m_data->buddyIncrease(userId,frinedId);
    QString ip=getUserIp(userId);
    if(ip.isEmpty())//用户不在线
    {
        return;
    }
    else
    {
        //用户在线
        TcpSocket *socket=m_SocketS.key(userId);
        connect(this,&QMServer::upList,
                socket,&TcpSocket::upList);
        QByteArray data=getUserInfo(frinedId);
        emit upList(data);
        disconnect(this,&QMServer::frined,
                socket,&TcpSocket::frined);
    }
}

void QMServer::on_pushButton_clicked()
{
    if(!ui->start->isEnabled())//正在监听
    {
        //Udp聊天室
        m_publicPort=m_ServerPort+3;
        m_publicSocket->bind(QHostAddress::AnyIPv4,m_publicPort,QUdpSocket::ShareAddress);
        m_publicAddress.setAddress("239.255.255.255");
        connect(m_publicSocket,&QUdpSocket::readyRead,
                this,&QMServer::readPublic);
        //只在局域网传播
        m_publicSocket->setSocketOption(QAbstractSocket::MulticastTtlOption,1);
        //组播回环
        m_publicSocket->setSocketOption(QAbstractSocket::MulticastLoopbackOption,false);

        //加入多播组
        m_publicSocket->joinMulticastGroup(m_publicAddress);
        ui->plainTextEdit->appendPlainText(tr("%1:聊天室开启")
                                           .arg(time()));
        QString name=tr("管理员");
        QString id="111111";
        listId.append(id);
        listName.append(name);
        QListWidgetItem *item=new QListWidgetItem;
        item->setText(tr("%1").arg(name));
        item->setData(2000,id);
        ui->listWidget->addItem(item);
        ui->lineEdit_4->setText(tr("%1").arg(listId.count()));
        ui->pushButton->setEnabled(false);
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
        //开启聊天室按钮不可选时，聊天室打开
        emit publiOpen(!ui->pushButton->isEnabled());
        isOpen=true;
    }
    else
    {
        QMessageBox::critical(this,tr("开启聊天室"),
                              tr("服务器未开启"));
    }
}

void QMServer::on_pushButton_2_clicked()
{
    m_publicSocket->leaveMulticastGroup(m_publicAddress);
    m_publicSocket->abort();
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    emit publiOpen(!ui->pushButton->isEnabled());
    TcpSocket *socket;
    //0是管理员,从1开始
    for(int i=1;i<listId.count();i++)
    {
        socket=m_SocketS.key(listId.at(i));
        connect(this,&QMServer::publiClear,
                socket,&TcpSocket::isManySpeakOpen);
        emit publiClear();
        disconnect(this,&QMServer::publiClear,
                   socket,&TcpSocket::isManySpeakOpen);
    }
    listId.clear();
    listName.clear();
    ui->listWidget->clear();
    ui->lineEdit_4->setText(tr("%1").arg(listId.count()));
    ui->plainTextEdit->appendPlainText(tr("%1:聊天室关闭")
                                       .arg(time()));
    isOpen=false;
    delete m_emoji;
    m_emoji=nullptr;
}

void QMServer::on_pushButton_3_clicked()
{//43
    QString msg=ui->textEdit_2->toPlainText();
    if(msg.isEmpty())
        return;
    msg=msg.trimmed();
    ui->textEdit_2->clear();
    ui->textEdit->setAlignment(Qt::AlignRight);
    ui->textEdit->append(tr("%1 %2:\n%3\n")
                         .arg(listName.at(0))
                         .arg(times())
                         .arg(msg));
    QByteArray info;
    QDataStream out(&info,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out<<listId.at(0)<<times()<<msg;
    quint8 type=43;
    sendPublic(type,info);
}

void QMServer::on_pushButton_4_clicked()
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
    m_emoji->setGeometry(11,400,420,140);
    m_emoji->setTableVisible();
    ui->pushButton_4->setIcon(QIcon(":/icon/bq1.png"));
    m_emoji->show();
}
