#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QProcess>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    bool server(true);

    if(argc > 1)
        server = false;

    MainWindow w(server);
    w.show();

    if(argc == 1)
    {
        // Start a duplicate process, that will connect to the server
        QProcess *p = new QProcess(&a);
        p->start(argv[0], QStringList("-n"));
    }

    return a.exec();
}
