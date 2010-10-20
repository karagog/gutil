#include "messageboxlogger.h"
#include "Core/exception.h"
#include <QMessageBox>
#include <QApplication>
using namespace GUtil;

Logging::MessageBoxLogger::MessageBoxLogger(QObject *parent)
    :AbstractLogger(parent)
{
}

void Logging::MessageBoxLogger::LogMessage_protected(const QByteArray &msg, MessageLevelEnum l)
{
    switch(l)
    {
    case Info:
        QMessageBox::information(0, "Info", QString(msg));
        break;
    case Warning:
        QMessageBox::warning(0, "Warning", QString(msg));
        break;
    case Error:
        QMessageBox::critical(0, "ERROR!", QString(msg));
        break;
    default:
        throw Core::NotImplementedException();
    }
}
