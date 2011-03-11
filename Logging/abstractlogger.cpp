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
#include "Core/exception.h"
#include "DataAccess/giodevice.h"
#include <QDateTime>
#include <QCoreApplication>
using namespace GUtil;

Logging::AbstractLogger::AbstractLogger(QObject *parent)
    :QObject(parent == 0 ? qApp : parent)
{
    _message_level = Info;

    // So passing exceptions through signals and slots will work
    qRegisterMetaType<GUtil::Core::Exception>("GUtil::Core::Exception");
}

Logging::AbstractLogger::~AbstractLogger()
{
}

void Logging::AbstractLogger::SetMessageLevel(MessageLevelEnum message_level)
{
    _message_level = message_level;
}

Logging::AbstractLogger::MessageLevelEnum Logging::AbstractLogger::MessageLevel()
{
    return _message_level;
}

void Logging::AbstractLogger::ClearLog()
{
    // Do nothing by default; subclasses define how to clear the log
}

void Logging::AbstractLogger::LogMessage(const QString &msg, const QString &title)
{
    Log(msg, title, Info);
}

void Logging::AbstractLogger::LogWarning(const QString &msg, const QString &title)
{
    Log(msg, title, Warning);
}

void Logging::AbstractLogger::LogError(const QString &msg, const QString &title)
{
    Log(msg, title, Error);
}

void Logging::AbstractLogger::LogException(const Core::Exception &ex)
{
    QString data_string;

    std::vector<std::string> keys = ex.GetDataKeys(true);
    if(keys.size() > 0)
    {
        data_string = "\n\nException Data:";

        for(std::vector<std::string>::const_iterator it = keys.begin();
        it != keys.end();
        it++)
        {
            std::string tmps(ex.GetData(*it));
            if(tmps.length() > 1000)
                tmps = tmps.substr(0, 1000) + "<--- TRUNCATED";

            data_string.append(QString("\n\tKey: %1   Value: %2")
                               .arg(QString::fromStdString(*it))
                               .arg(QString::fromStdString(tmps)));
        }
    }

    Log(QString("%1%2")
        .arg(QString::fromStdString(ex.GetMessage()))
        .arg(data_string),

        QString("%1 Caught%2:")
        .arg(QString::fromStdString(ex.ToString()))
        .arg(ex.GetInnerException() ? " (Inner exception follows immediately)" : ""),

        Error);

    if(ex.GetInnerException())
        LogException(*ex.GetInnerException());
}

void Logging::AbstractLogger::LogException(const std::exception &)
{
    Log(QString::null, "Exception Caught: std::exception", Error);
}

void Logging::AbstractLogger::Log(const QString &msg, const QString &title, MessageLevelEnum message_level)
{
    if(message_level < MessageLevel())
        return;

    QString log_message = prepare_log_message(msg, title, message_level);

    try
    {
        Log_protected(log_message.toAscii(), message_level);
    }
    catch(...)
    {
        return;
    }
}

QString Logging::AbstractLogger::prepare_log_message(
        const QString &msg,
        const QString &title,
        Logging::AbstractLogger::MessageLevelEnum message_type,
        const QDateTime &dt)
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
        THROW_NEW_GUTIL_EXCEPTION2( Core::NotImplementedException, "Invalid Message Type" );
    }

    return QString("%1  %2: %3%4\n\n\n")
            .arg(dt.toString(Qt::ISODate))
            .arg(msg_id)
            .arg(title)
            .arg(msg.length() == 0 ? QString::null : QString("\n%1").arg(msg));
}
