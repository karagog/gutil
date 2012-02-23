/*Copyright 2010-2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "extendedexception.h"
#include "abstractlogger.h"
#include "DataObjects/map.h"
#include <time.h>
USING_NAMESPACE_GUTIL1(DataAccess);
USING_NAMESPACE_GUTIL1(DataObjects);

NAMESPACE_GUTIL1(Logging)


AbstractLogger::AbstractLogger(OutputInterface *io)
    :_io(io),
      m_logExceptionDetails(true)
{
    m_message_level_tolerance = Info;
}

void AbstractLogger::LogExceptionDetails(bool l)
{
    m_logExceptionDetails = l;
}

void AbstractLogger::SetMessageLevelTolerance(MessageLevelEnum message_level)
{
    m_message_level_tolerance = message_level;
}

AbstractLogger::MessageLevelEnum AbstractLogger::MessageLevelTolerance()
{
    return m_message_level_tolerance;
}


#define TRUNCATE_LIMIT 1000

void AbstractLogger::LogException(const Exception<false> &ex)
{
    const Exception<true> *ex_ptr( dynamic_cast<const Exception<true> *>(&ex) );

    if(ex_ptr)
    {
        String data_string;
        if(m_logExceptionDetails)
        {
            const Map<String, String> &keys( ex_ptr->GetDataMap() );
            if(keys.Size() > 0)
            {
                data_string = "\n\nException Data:";

                for(Map<String, String>::const_iterator it( keys.begin() );
                    it; it++)
                {
                    String tmps( it->Value() );
                    if(tmps.Length() > TRUNCATE_LIMIT)
                        tmps.TruncateUTF8(TRUNCATE_LIMIT)
                                .Append(String::Format(
                                            "<--- TRUNCATED AT %d CHARACTERS",
                                            TRUNCATE_LIMIT));

                    data_string.Append(String::Format("\n\tKey: %s   Value: %s",
                                                      it->Key().ConstData(),
                                                      tmps.ConstData()));
                }
            }
        }

        Log(String::Format("%s%s", ex_ptr->GetMessage(), data_string.ConstData()),

            String::Format("%s Caught from line %d of file '%s'%s:",
            ex.What ? ex.What : "[ null ]",
            ex.Line,
            ex.File ? ex.File : "[ no file ]",
            ex_ptr->GetInnerException() ? " (Inner exception follows immediately)" : ""),

            Error);

        if(ex_ptr->GetInnerException())
            LogException(*ex_ptr->GetInnerException());
    }
    else
    {
        Log("",
            String::Format("%s Caught from line %d of file '%s':",
                           ex.What ? ex.What : "[ null ]",
                           ex.Line,
                           ex.File ? ex.File : "[ no file ]"),
            Error);
    }
}

void AbstractLogger::Log(const String &msg, const String &title, MessageLevelEnum message_level)
{
    if(message_level < MessageLevelTolerance())
        return;

    String log_message( prepare_log_message(msg, title, message_level) );
    try
    {
        _io->WriteBytes(reinterpret_cast<GBYTE const *>(log_message.ConstData()),
                        log_message.Length());
    }
    catch(...)
    {
        return;
    }
}

String AbstractLogger::prepare_log_message(
        const String &msg,
        const String &title,
        MessageLevelEnum message_type)
{
    char const *msg_id;
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
        THROW_NEW_GUTIL_EXCEPTION2( NotImplementedException, "Invalid Message Type" );
    }

    // Find the current time
    time_t current_time( time(NULL) );
    char tm_buf[30] = {'I', '\0'};
    strftime(tm_buf, 30, "%Y-%m-%d %H:%M:%S", localtime(&current_time));

    return String::Format("%s  %s: %s%s\n\n\n",
                          tm_buf, msg_id,
                          title.Length() == 0 ? "" : title.ConstData(),
                          msg.Length() == 0 ? "" : String(1 + msg.Length()).Append("\n").Append(msg).ConstData());
}


END_NAMESPACE_GUTIL1;
