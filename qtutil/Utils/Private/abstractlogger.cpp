#include "abstractlogger.h"
#include "pubsubsystem.h"
#include "exception.h"
#include <QDateTime>
using namespace GQtUtil::Utils;
using namespace GUtil;

AbstractLogger::AbstractLogger(PubSubSystem *pss, QObject *parent)
    :QObject(parent)
{
    _pubsub = pss;

    if(pss != 0)
        connect(pss, SIGNAL(notifyMessage(QString,QString)), this, SLOT(LogMessage(QString,QString)));
}

void AbstractLogger::LogMessage(const QString &msg)
{
    LogMessage(msg, QString::null);
}

void AbstractLogger::LogMessage(const QString &msg, const QString &title)
{
    Log(msg, title, Message);
}

void AbstractLogger::LogError(const QString &msg)
{
    LogError(msg, QString::null);
}

void AbstractLogger::LogError(const QString &msg, const QString &title)
{
    Log(msg, title, Error);
}

void AbstractLogger::LogException(const GUtil::Exception &ex)
{
    Log(QString::fromStdString(ex.Message()),
        QString("Exception Caught: %1").arg(QString::fromStdString(ex.ToString())),
        Error);
}

void AbstractLogger::Log(const QString &msg, const QString &title, MessageTypeEnum message_type)
{
    QString log_message = PrepareLogMessage(msg, title, message_type);

    LogMessage_protected(log_message, message_type);

    emit NotifyMessageLogged(log_message, message_type);
}

QString AbstractLogger::PrepareLogMessage(const QString &msg, const QString &title, MessageTypeEnum message_type)
{
    QString msg_id;
    switch(message_type)
    {
    case Message:
        msg_id = "Info";
        break;
    case Error:
        msg_id = "ERROR";
        break;
    default:
        throw NotImplementedException("Invalid Message Type");
    }

    return QString("%1  %2: %3\n%4\n\n")
            .arg(QDateTime::currentDateTime().toString(Qt::DefaultLocaleLongDate))
            .arg(msg_id)
            .arg(title)
            .arg(msg);
}
