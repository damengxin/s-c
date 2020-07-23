#include "qmemoji.h"
#include "ui_qmemoji.h"

QMEmoji::QMEmoji(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QMEmoji)
{
    ui->setupUi(this);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(ui->tableWidget,SIGNAL(visible()),
            this,SIGNAL(visible()));
}

QMEmoji::~QMEmoji()
{
    delete ui;
}

void QMEmoji::setTableVisible()
{
    ui->tableWidget->setCurrentItem(ui->tableWidget->item(0,0));
    ui->tableWidget->setFocus();
    ui->tableWidget->setVisible(true);
}

void QMEmoji::on_tableWidget_itemClicked(QTableWidgetItem *item)
{
    QString s=item->text();
    emit emoji(s);
}
