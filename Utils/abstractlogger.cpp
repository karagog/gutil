/*Copyright 2010 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "abstractlogger.h"
#include "pubsubsystem.h"
#include "Core/exception.h"
#include "DataAccess/DataTransports/abstractdatatransportmechanism.h"
#include <QDateTime>
using namespace GUtil;

Utils::AbstractLogger::AbstractLogger(Utils::PubSubSystem *pss, QObject *parent)
    :QObject(parent)
{
    _pubsub = pss;

    _message_level = Info;

    if(pss != 0)
    {
        connect(pss, SIGNAL(NotifyMessage(QString,QString)), this, SLOT(LogMessage(QString,QString)));
        connect(pss, SIGNAL(NotifyWarning(QString,QString)), this, SLOT(LogWarning(QString,QString)));
        connect(pss, SIGNAL(NotifyError(QString,QString)), this, SLOT(LogError(QString,QString)));
    }
}

Utils::AbstractLogger::~AbstractLogger()
{

}

void Utils::AbstractLogger::SetMessageLevel(MessageLevelEnum message_level)
{
    _message_level = message_level;
}

Utils::AbstractLogger::MessageLevelEnum Utils::AbstractLogger::GetMessageLevel()
{
    return _message_level;
}

void Utils::AbstractLogger::LogMessage(const QString &msg, const QString &title)
{
    Log(msg, title, Info);
}

void Utils::AbstractLogger::LogWarning(const QString &msg, const QString &title)
{
    Log(msg, title, Warning);
}

void Utils::AbstractLogger::LogError(const QString &msg, const QString &title)
{
    Log(msg, title, Error);
}

void Utils::AbstractLogger::LogException(const Core::Exception &ex)
{
    QString data_string;

    std::vector<std::string> keys = ex.GetDataKeys(true);
    if(keys.size() > 0)
    {
        data_string = "\n\nException Data:";

        for(std::vector<std::string>::const_iterator it = keys.begin(); it != keys.end(); it++)
            data_string.append(QString("\n\tKey: %1   Value: %2")
                               .arg(QString::fromStdString(*it))
                               .arg(QString::fromStdString(ex.GetData(*it))));
    }

    Log(QString("%1%2").arg(QString::fromStdString(ex.Message())).arg(data_string),
        QString("Exception Caught: %1").arg(QString::fromStdString(ex.ToString())),
        Error);
}

void Utils::AbstractLogger::LogException(const std::exception &ex)
{
	Log(QString::null, "Exception Caught: std::exception", Error);
}

void Utils::AbstractLogger::Log(const QString &msg, const QString &title, MessageLevelEnum message_level)
{
    if(message_level < _message_level)
        return;

    QString log_message = PrepareLogMessage(msg, title, message_level);

    try
    {
        LogMessage_protected(log_message.toAscii(), message_level);
    }
    catch(...)
    {
        return;
    }

    emit NotifyMessageLogged(log_message, message_level);
}

QString Utils::AbstractLogger::PrepareLogMessage(const QString &msg, const QString &title, Utils::AbstractLogger::MessageLevelEnum message_type, const QDateTime &dt)
{
    QString msg_id;
    switch(message_type)
    {
    case Info:
        msg_id = "Info";
        break;
    case Warning:
        msg_id = "Warning";
        break;
    case Error:
        msg_id = "ERROR";
        break;
    default:
        throw Core::NotImplementedException("Invalid Message Type");
    }

    return QString("%1  %2: %3%4\n\n\n")
            .arg(dt.toString(Qt::ISODate))
            .arg(msg_id)
            .arg(title)
            .arg(msg.length() == 0 ? QString::null : QString("\n%1").arg(msg));
}

//bool Utils::AbstractLogger::PreLogMessage()
//{
//    // Do nothing by default
//    return true;
//}

void Utils::AbstractLogger::LogMessage_protected(const QByteArray &log_message, MessageLevelEnum)
{
    TransportMechanism().SendData(log_message);
}

//void Utils::AbstractLogger::PostLogMessage()
//{
//    // Do nothing by default
//}
