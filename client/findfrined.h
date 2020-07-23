#ifndef FINDFRINED_H
#define FINDFRINED_H

#include <QWidget>

namespace Ui {
class findFrined;
}

class findFrined : public QWidget
{
    Q_OBJECT

public:
    explicit findFrined(QWidget *parent = nullptr);
    ~findFrined();
    void setFineUser(QByteArray data,bool b=true);
    void setUnreadInfo(QByteArray data);
    void setFineVisible(bool b);

private slots:
    void on_pushButton_clicked();
    void clear_Button();

signals:
    void find(quint8 type,QString id); //25
    void add(quint8 type,QString id);  //2
    void agree(quint8 type,QString id);//4


private:
    Ui::findFrined *ui;
};

#endif // FINDFRINED_H
