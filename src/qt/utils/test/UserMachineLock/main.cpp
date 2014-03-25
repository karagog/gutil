#include <QtGui/QApplication>
#include <QProcess>
#include "mainwindow.h"
#include "Logging/globallogger.h"
#include "Logging/messageboxlogger.h"
using namespace GUtil;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GlobalLogger::SetDefaultLoggerID(GlobalLogger::SetupLogger(new MessageBoxLogger(&a)));

    if(argc == 1)
        (new QProcess(&a))->start(QString(argv[0]), QStringList("-n"));

    MainWindow w1;
    w1.show();

    return a.exec();
}
