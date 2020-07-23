#ifndef FORGET_H
#define FORGET_H

#include <QWidget>

namespace Ui {
class forget;
}

class forget : public QWidget
{
    Q_OBJECT

public:
    explicit forget(QWidget *parent = nullptr);
    ~forget();
    QString encrypt(QString &str);
    void setResult(QByteArray info);
    void setVisi(bool b);

private slots:
    void on_pushButton_clicked();
signals:
    void for_get(quint8,QByteArray);

private:
    Ui::forget *ui;
};

#endif // FORGET_H
