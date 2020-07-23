#include "qmavatar.h"
#include "ui_qmavatar.h"
#include <QListView>
#include <QFile>
#include <QDebug>

QMavatar::QMavatar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QMavatar)
{
    ui->setupUi(this);
    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setIconSize(QSize(90,90));
    ui->listWidget->setMovement(QListView::Static);
    QString filename;
    for(int i=0;i<8;i++)
    {
        filename=QString(":/icon/tx%1.jpg").arg(i+1);
        QFile fi(filename);
        fi.open(QIODevice::ReadOnly);
        QByteArray photo;
        photo=fi.readAll();
        fi.close();
        m_li.append(photo);
        QListWidgetItem *item=new QListWidgetItem;
        item->setText(tr("默认%1").arg(i+1));
        QPixmap map;
        map.loadFromData(photo);
        item->setIcon(QIcon(map));
        ui->listWidget->addItem(item);
    }
}

QMavatar::~QMavatar()
{
    delete ui;
}

void QMavatar::on_listWidget_itemClicked(QListWidgetItem *item)
{
    Q_UNUSED(item)
    int index=ui->listWidget->currentIndex().row();
    photo=m_li.at(index);        //手动获取
    emit iconinfo(getIconInfo());//槽函数获取
}
