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

#ifndef GUTIL_GLOBALLOGGER_H
#define GUTIL_GLOBALLOGGER_H

#include "gutil_exception.h"
#include "gutil_strings.h"
#include "gutil_map.h"
#include "gutil_smartpointer.h"
#include "abstractlogger.h"

NAMESPACE_GUTIL1(Logging);


/** A static class that facilitates a global logging entity, accessible
    from anywhere inside your code.

    You can simply call GlobalLogger::LogMessage(...),
    and if there is a global logger set up then it will log,
    otherwise it will silently and efficiently ignore it.
*/
class GlobalLogger
{
public:

    /** Use this to control the behavior of setting up a new global logger. */
    enum LoggerIdEnum
    {
        /** This will set up the debug logger, which is a special logger only
            used in debug mode.
        */
        DebugId = -3,

        /** This will set it up with the default logger id. */
        DefaultId = -2,

        /** This will cause a new ID to be assigned and returned.  Use
            this if you don't want to pick your own ids.
        */
        NewId = -1
    };


    /** Sets up the logger with the corresponding logger id.
        If you pass NewId then a new ID will be assigned and returned
        after the logger is inserted.  You will then use this ID to
        reference this now-global logger.
    */
    static int SetupLogger(AbstractLogger *l, int logger_id = NewId){
        _translate_logger_id(logger_id, true);
        _takedown_logger(logger_id);
        m_loggers.Insert(logger_id, l);
        return logger_id;
    }

    /** Sets up the logger with the default ID. */
    static int SetupDefaultLogger(AbstractLogger *l){
        return SetupLogger(l, DefaultId);
    }

    /** Removes the logger from the global set and deletes it. */
    static void TakeDownLogger(int logger_id = DefaultId){
        _translate_logger_id(logger_id, false);
        _takedown_logger(logger_id);
    }

    /** Changes the default logger ID, so all future messages go to a different logger. */
    static void SetDefaultLoggerID(int new_id){
        _translate_logger_id(new_id, false);
        if(m_loggers.Contains(new_id))
            m_default_logger_id = new_id;
    }

    /** Returns the current default logger id. */
    static int GetDefaultLoggerID(){ return m_default_logger_id; }



    /** Logs a message. */
    static void LogMessage(const DataObjects::String &msg,
                                  const DataObjects::String &title = DataObjects::String(),
                                  int logger_id = DefaultId)
    {
        _log(msg, title, logger_id, AbstractLogger::MessageLevel_Info);
    }

    /** Logs a message with a severity of "warning". */
    static void LogWarning(const DataObjects::String &msg,
                           const DataObjects::String &title = DataObjects::String(),
                           int logger_id = DefaultId)
    {
        _log(msg, title, logger_id, AbstractLogger::MessageLevel_Warning);
    }

    /** Logs a message with a severity of "error". */
    static void LogError(const DataObjects::String &msg,
                                const DataObjects::String &title = DataObjects::String(),
                                int logger_id = DefaultId)
    {
        _log(msg, title, logger_id, AbstractLogger::MessageLevel_Error);
    }

    /** Logs an exception. */
    static void LogException(const Exception<false> &ex, int logger_id = DefaultId){
        _translate_logger_id(logger_id, false);
        if(m_loggers.Contains(logger_id))
        {
            try{
                m_loggers[logger_id]->LogException(ex);
            }catch(...){}
        }
    }


private:

    static DataObjects::Map<int, Utils::SharedSmartPointer<AbstractLogger> > m_loggers;
    static int m_default_logger_id;


    static void _translate_logger_id(int &, bool allow_new_id);

    static void _takedown_logger(int logger_id){
        if(m_loggers.Contains(logger_id)){
            m_loggers.Remove(logger_id);
        }
    }

    static void _log(const DataObjects::String &msg,
                            const DataObjects::String &title,
                            int logger_id,
                            AbstractLogger::MessageLevelEnum ml)
    {
        _translate_logger_id(logger_id, false);
        if(m_loggers.Contains(logger_id))
        {
            AbstractLogger::LoggingData d;
            d.Message = msg;
            d.Title = title;
            d.MessageLevel = ml;
            try{
                m_loggers[logger_id]->Log(d);
            }catch(...){}
        }
    }

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_GLOBALLOGGER_H
