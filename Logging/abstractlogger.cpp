/*Copyright 2011 George Karagoulis

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
#include "Core/extendedexception.h"
#include "DataAccess/giodevice.h"
#include <QDateTime>
#include <QCoreApplication>
GUTIL_USING_NAMESPACE(Logging);
GUTIL_USING_CORE_NAMESPACE(DataObjects);

AbstractLogger::AbstractLogger(QObject *parent)
    :QObject(parent == 0 ? qApp : parent),
      m_logExceptionDetails(true)
{
    _message_level = Info;

    // So passing exceptions through signals and slots will work
    qRegisterMetaType< GUtil::Core::Exception<false> >("GUtil::Core::Exception<false");
    qRegisterMetaType< GUtil::Core::Exception<true> >("GUtil::Core::Exception<true>");
}

AbstractLogger::~AbstractLogger()
{}

void AbstractLogger::LogExceptionDetails(bool l)
{
    m_logExceptionDetails = l;
}

void AbstractLogger::SetMessageLevel(MessageLevelEnum message_level)
{
    _message_level = message_level;
}

AbstractLogger::MessageLevelEnum AbstractLogger::MessageLevel()
{
    return _message_level;
}

void AbstractLogger::ClearLog()
{
    // Do nothing by default; subclasses define how to clear the log
}

void AbstractLogger::LogMessage(const QString &msg, const QString &title)
{
    Log(msg, title, Info);
}

void AbstractLogger::LogWarning(const QString &msg, const QString &title)
{
    Log(msg, title, Warning);
}

void AbstractLogger::LogError(const QString &msg, const QString &title)
{
    Log(msg, title, Error);
}

#define TRUNCATE_LIMIT 1000

void AbstractLogger::LogException(const Core::Exception<> &ex)
{
    const Core::Exception<true> *ex_ptr( dynamic_cast<const Core::Exception<true> *>(&ex) );

    if(ex_ptr)
    {
        QString data_string;
        if(m_logExceptionDetails)
        {
            const Map<String, String> &keys( ex_ptr->GetDataMap() );
            if(keys.Size() > 0)
            {
                data_string = "\n\nException Data:";

                for(Map<String, String>::const_iterator it( keys.begin() );
                    it; it++)
                {
                    QString tmps(it->Value().ConstData());
                    if(tmps.length() > TRUNCATE_LIMIT)
                        tmps = tmps.left(TRUNCATE_LIMIT) +
                                QString("<--- TRUNCATED AT %1 CHARACTERS")
                                .arg(TRUNCATE_LIMIT);

                    data_string.append(QString("\n\tKey: %1   Value: %2")
                                       .arg(it->Key().ConstData())
                                       .arg(tmps));
                }
            }
        }

        Log(QString("%1%2")
            .arg(ex_ptr->GetMessage())
            .arg(data_string),

            QString("%1 Caught from line %2 of file '%3'%4:")
            .arg(ex.What ? ex.What : "[ null ]")
            .arg(ex.Line)
            .arg(ex.File ? ex.File : "[ no file ]")
            .arg(ex_ptr->GetInnerException() ? " (Inner exception follows immediately)" : ""),

            Error);

        if(ex_ptr->GetInnerException())
            LogException(*ex_ptr->GetInnerException());
    }
    else
    {
        Log("", QString("%1 Caught from line %2 of file '%3':")
            .arg(ex.What ? ex.What : "[ null ]")
            .arg(ex.Line)
            .arg(ex.File ? ex.File : "[ no file ]"),
            Error);
    }
}

void AbstractLogger::LogException(const std::exception &)
{
    Log(QString::null, "Exception Caught: std::exception", Error);
}

void AbstractLogger::Log(const QString &msg, const QString &title, MessageLevelEnum message_level)
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

QString AbstractLogger::prepare_log_message(
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
