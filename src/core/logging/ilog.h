/*Copyright 2014 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_ILOG_H
#define GUTIL_ILOG_H

#include <gutil/string.h>
#include <time.h>

namespace std{
class exception;
}

namespace GUtil{


/** The interface for logging classes. */
class ILog
{
public:

    /** \name Main usage
     *  These are the main functions you will use as a consumer of the ILog
     *  \{
    */

    /** Logs a message with the lowest severity */
    inline void LogInfo(const char *message, const char *title = NULL){
        Log(LoggingData(message, title, LogLevelInfo));
    }

    /** Logs a warning */
    inline void LogWarning(const char *message, const char *title = NULL){
        Log(LoggingData(message, title, LogLevelWarning));
    }

    /** Logs an error */
    inline void LogError(const char *message, const char *title = NULL){
        Log(LoggingData(message, title, LogLevelError));
    }

    /** Logs the exception with all its details.
     *  The default implementation prepares the logging data then calls Log().
    */
    virtual void LogException(const std::exception &) noexcept;

    /** \} */


    /** Enumerates various log levels, under which log messages will
        be ignored.
    */
    enum LogLevelEnum
    {
        /** Logs all messages, even info. */
        LogLevelInfo,

        /** Logs only messages marked as "warning" or worse. */
        LogLevelWarning,

        /** Logs only messages marked as "error" */
        LogLevelError
    };

    /** Something to be logged.  Contains all the information needed to log an event. */
    struct LoggingData
    {
        String Message;
        String Title;
        LogLevelEnum MessageLevel;
        time_t LogTime = time(NULL);

        LoggingData() {}
        LoggingData(const char *message, const char *title, LogLevelEnum log_level)
            :Message(message),
              Title(title),
              MessageLevel(log_level)
        {}
    };

    /** Logs the message. */
    virtual void Log(const LoggingData &) noexcept = 0;

    /** Controls the log level, which determines which messages get ignored and
     *  which get logged.
    */
    PROPERTY(LogLevel, LogLevelEnum);

    /** Clears the log (if applicable). */
    virtual void Clear() {}

    /** You can be deleted by this interface. */
    virtual ~ILog() {}


protected:

    /** Returns true if we should log the data.
     *  By default we should log all messages over the log level.
    */
    virtual bool should_log(const LoggingData &d){
        return d.MessageLevel >= GetLogLevel();
    }

    ILog() :_p_LogLevel(LogLevelInfo) {}

};


}

#endif // GUTIL_ILOG_H
