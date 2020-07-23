#ifndef QMLOGIN_H
#define QMLOGIN_H

#include <QDialog>
#include <QSqlDatabase>

#include "qmserver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class QMLogin; }
QT_END_NAMESPACE

class QMLogin : public QDialog
{
    Q_OBJECT

public:
    QMLogin(QWidget *parent = nullptr);
    ~QMLogin();
    bool login(QString &id,QString &pass);

    static QString encrypt(QString &str);
private slots:
    void on_pushButton_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_lineEdit_2_textChanged(const QString &arg1);

private:
    Ui::QMLogin *ui;
    QMServer *m_server;
    QSqlDatabase db;
    QString r="001";
    QString p="b961294ef473636627d3e80c394503c62"
              "834d384c698ef459854a56e17c8866259"
              "7f3ceea13be03fc34e27a428c1bef6be5"
              "af2aa26bd1954eca073f10a9b6eaa";
};
#endif // QMLOGIN_H
