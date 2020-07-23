#include "userchat.h"
#include "ui_userchat.h"

userChat::userChat(QString name,QWidget *parent) :
    QWidget(parent),ui(new Ui::userChat),
    m_userName(name)
{
    ui->setupUi(this);
    ui->textEdit_2->setFocus();
}

userChat::~userChat()
{
    delete ui;
}

void userChat::setUserNameId(QString name)
{
    setWindowTitle(name);
}

void userChat::setChat(QString &time, QString &msg)
{
    ui->textEdit->setAlignment(Qt::AlignLeft);
    ui->textEdit->append(tr("%1 %2:\n%3\n")
                         .arg(windowTitle())
                         .arg(time)
                         .arg(msg));
    ui->textEdit_2->setFocus();
    emit msgInfo(msg.left(10)+"...");
}

void userChat::up(QString name)
{
    m_userName=name;
}

void userChat::emojiMsg(QString s)
{
    QString text=ui->textEdit_2->toPlainText();
    ui->textEdit_2->setText(text+s);
}

void userChat::visible()
{
    ui->pushButton_2->setIcon(QIcon(":/icon/bq.png"));
}

void userChat::on_pushButton_clicked()
{
    QString msg=ui->textEdit_2->toPlainText();
    ui->textEdit_2->clear();
    if(msg.isEmpty())
        return;
    msg=msg.trimmed();
    ui->textEdit->setAlignment(Qt::AlignRight);
    ui->textEdit->append(tr("%1 %2:\n%3\n")
                         .arg(m_userName)
                         .arg(times())
                         .arg(msg));
    quint8 type=11;
    emit udpInfo(type,msg);
    emit msgInfo(msg.left(10)+"...");
    ui->textEdit_2->setFocus();
}

void userChat::on_pushButton_2_clicked()
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
    m_emoji->setGeometry(11,200,420,140);
    m_emoji->setTableVisible();
    ui->pushButton_2->setIcon(QIcon(":/icon/bq1.png"));
    m_emoji->show();
}
