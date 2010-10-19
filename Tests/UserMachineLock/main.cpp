#include <QtGui/QApplication>
#include <QProcess>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(argc == 1)
        (new QProcess(&a))->start(QString(argv[0]), QStringList("-n"));

    MainWindow w1;
    w1.show();

    return a.exec();
}
