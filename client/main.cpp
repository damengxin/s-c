#include "qmlogin.h"
#include "qmuser.h"
#include "userpanel.h"
#include "newuser.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMLogin w;
    //QMUser w;
    //userPanel w;
    //newUser w;
    w.show();
    return a.exec();
}
