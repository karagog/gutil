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

#include "gutil_extendedexception.h"
#include "gutil_abstractlogger.h"
#include "gutil_map.h"
USING_NAMESPACE_GUTIL1(DataAccess);
USING_NAMESPACE_GUTIL1(DataObjects);

NAMESPACE_GUTIL1(Logging)


#define DEFAULT_TRUNCATE_LIMIT   1000

AbstractLogger::AbstractLogger(OutputInterface *io)
    :_io(io),
      m_options(GUINT32_MAX),
      m_truncate_limit(DEFAULT_TRUNCATE_LIMIT)
{}

AbstractLogger::AbstractLogger(OutputInterface *io, const AbstractLogger::LoggingOptionsFlags &f)
    :_io(io),
      m_options(f),
      m_truncate_limit(DEFAULT_TRUNCATE_LIMIT)
{}

void AbstractLogger::Clear(){}

void AbstractLogger::LogException(const Exception<false> &ex)
{
    if(!should_log_message(MessageLevel_Error))
        return;

    ExtendedException const *ex_ptr( dynamic_cast<ExtendedException const *>(&ex) );

    if(ex_ptr)
    {
        String data_string;
        if(m_options.TestFlag(Option_LogExceptionDetails))
        {
            const Map<String, String> &keys( ex_ptr->GetDataMap() );
            if(keys.Size() > 0)
            {
                data_string = "\n\nException Data:";

                for(Map<String, String>::const_iterator it( keys.begin() );
                    it; ++it)
                {
                    String tmps( it->Value() );
                    if(m_options.TestFlag(Option_TruncateExceptionDetails) &&
                            tmps.Length() > m_truncate_limit)
                    {
                        tmps.TruncateUTF8(m_truncate_limit)
                                .Append(String::Format(
                                            "<--- TRUNCATED AT %d CHARACTERS",
                                            m_truncate_limit));
                    }

                    data_string.Append(String::Format("\n\tKey: %s   Value: %s",
                                                      it->Key().ConstData(),
                                                      tmps.ConstData()));
                }
            }
        }

        Log(String::Format("%s%s", ex.GetMessage(), data_string.ConstData()),
            String::Format("%s caught from line %d of file '%s'%s:",
                           ex.What ? ex.What : "[ null ]",
                           ex.Line,
                           ex.File ? ex.File : "[ no file ]",
                           ex_ptr->GetInnerException() ? " (Inner exception follows immediately)" : ""),
            MessageLevel_Error);

        if(ex_ptr->GetInnerException())
            LogException(*ex_ptr->GetInnerException());
    }
    else
    {
        Log(ex.GetMessage() ? ex.GetMessage() : "",
            String::Format("%s caught from line %d of file '%s':",
                           ex.What ? ex.What : "[ null ]",
                           ex.Line,
                           ex.File ? ex.File : "[ no file ]"),
            MessageLevel_Error);
    }
}

void AbstractLogger::log_protected(const String &msg, const String &title, MessageLevelEnum message_level, time_t t)
{
    String log_message( prepare_log_message(msg, title, message_level, t) );
    try
    {
        _io->WriteBytes(reinterpret_cast<GBYTE const *>(log_message.ConstData()),
                        log_message.Length());
    }
    catch(...)
    {
        // Quietly absorb any exceptions.
        // We're just a logger, so we want to be sure we're stable
    }
}

String AbstractLogger::prepare_log_message(
        const String &msg,
        const String &title,
        MessageLevelEnum message_type,
        time_t current_time)
{
    char const *msg_id;
    switch(message_type)
    {
    case MessageLevel_Info:
        msg_id = "Info";
        break;
    case MessageLevel_Warning:
        msg_id = "Warning";
        break;
    case MessageLevel_Error:
        msg_id = "ERROR";
        break;
    default:
        THROW_NEW_GUTIL_EXCEPTION2( NotImplementedException,
                                    "Invalid Message Type" );
    }

    char tm_buf[30] = {'I', '\0'};
    strftime(tm_buf, 30, "%Y-%m-%d %H:%M:%S", localtime(&current_time));

    return String::Format("%s  %s: %s%s\n\n\n",
                          tm_buf, msg_id,
                          title.Length() == 0 ? "" : title.ConstData(),
                          msg.Length() == 0 ? "" : String(1 + msg.Length()).Append("\n").Append(msg).ConstData());
}


END_NAMESPACE_GUTIL1;
