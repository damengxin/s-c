#ifndef UPPASS_H
#define UPPASS_H

#include <QWidget>

namespace Ui {
class upPass;
}

class upPass : public QWidget
{
    Q_OBJECT

public:
    explicit upPass(QWidget *parent = nullptr);
    ~upPass();
    QString encrypt(QString &str);

private slots:
    void on_pushButton_clicked();

signals:
    void pass(QString);

private:
    Ui::upPass *ui;
};

#endif // UPPASS_H
