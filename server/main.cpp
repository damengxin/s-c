#include "qmlogin.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMLogin w;
    //QMServer w;
    w.show();
    return a.exec();
}
