/*Copyright 2010-2013 George Karagoulis

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
USING_NAMESPACE_GUTIL;
using namespace std;

NAMESPACE_GUTIL;


#define DEFAULT_TRUNCATE_LIMIT   1000

AbstractLogger::AbstractLogger(IOutput *io)
    :_io(io),
      m_options(GUINT32_MAX),
      m_truncate_limit(DEFAULT_TRUNCATE_LIMIT)
{}

AbstractLogger::AbstractLogger(IOutput *io, const AbstractLogger::LoggingOptionsFlags &f)
    :_io(io),
      m_options(f),
      m_truncate_limit(DEFAULT_TRUNCATE_LIMIT)
{}

void AbstractLogger::Clear(){}

void AbstractLogger::LogException(const std::exception &std_ex)
{
    LoggingData d;
    d.MessageLevel = MessageLevel_Error;
    if(!should_log_message(d))
        return;

    Exception<> const *ex = dynamic_cast<Exception<false> const *>(&std_ex);
    ExtendedException const *ext_ex( dynamic_cast<ExtendedException const *>(&std_ex) );
    if(ext_ex)
    {
        String data_string;
        if(m_options.TestFlag(Option_LogExceptionDetails))
        {
            const map<string, string> &keys( ext_ex->Data );
            if(keys.size() > 0)
            {
                data_string = "\n\nException Data:";
                for(const auto &p : keys){
                    String tmps = String::FromStdString(p.second);
                    if(m_options.TestFlag(Option_TruncateExceptionDetails) &&
                            tmps.Length() > m_truncate_limit)
                    {
                        tmps.TruncateUTF8(m_truncate_limit)
                                .Append(String::Format(
                                            "<--- TRUNCATED AT %d CHARACTERS",
                                            m_truncate_limit));
                    }

                    data_string.Append(String::Format("\n\tKey: %s   Value: %s",
                                                      p.first.data(),
                                                      tmps.ConstData()));
                }
            }
        }

        d.Message = String::Format("%s%s", ex->Message(), data_string.ConstData());
        d.Title = String::Format("%s caught from line %d of file '%s'%s:",
                                 ex->what(),
                                 ex->Line(),
                                 ex->File() ? ex->File() : "[ no file ]",
                                 ext_ex->GetInnerException() ? " (Inner exception follows immediately)" : "");
        this->Log(d);

        if(ext_ex->GetInnerException())
            AbstractLogger::LogException(*ext_ex->GetInnerException());
    }
    else if(ex)
    {
        d.Message = ex->Message() ? ex->Message() : "";
        d.Title = String::Format("%s caught from line %d of file '%s':",
                                 ex->what(),
                                 ex->Line(),
                                 ex->File() ? ex->File() : "[ no file ]");
        this->Log(d);
    }
    else
    {
        d.Title = String::Format("Caught %s", std_ex.what());
        this->Log(d);
    }
}

void AbstractLogger::Log(const LoggingData &d)
{
    if(this->should_log_message(d))
        this->log_protected(d);
}

void AbstractLogger::log_protected(const LoggingData &d)
{
    if(_io)
    {
        String log_message( prepare_log_message(d) );
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
}

bool AbstractLogger::should_log_message(const LoggingData &d)
{
    return m_options.TestFlag((LoggingOptionsEnum)d.MessageLevel);
}

String AbstractLogger::prepare_log_message(const LoggingData &d)
{
    char const *msg_id;
    switch(d.MessageLevel)
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
    strftime(tm_buf, 30, "%Y-%m-%d %H:%M:%S", localtime(&d.LogTime));

    return String::Format("%s  %s: %s%s\n\n\n",
                          tm_buf, msg_id,
                          d.Title.Length() == 0 ? "" : d.Title.ConstData(),
                          d.Message.Length() == 0 ? "" : String(1 + d.Message.Length()).Append("\n").Append(d.Message).ConstData());
}


END_NAMESPACE_GUTIL;
