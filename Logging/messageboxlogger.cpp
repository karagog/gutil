#include "messageboxlogger.h"
#include "Core/exception.h"
#include <QMessageBox>
using namespace GUtil;

Logging::MessageBoxLogger::MessageBoxLogger(QObject *parent)
    :AbstractLogger(parent)
{
}

Logging::MessageBoxLogger::~MessageBoxLogger()
{

}

void Logging::MessageBoxLogger::Log_protected(const QByteArray &msg, MessageLevelEnum l)
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
        THROW_GUTIL_EXCEPTION( Core::NotImplementedException() )
    }
}
