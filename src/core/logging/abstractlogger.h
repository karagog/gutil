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

#ifndef GUTIL_ABSTRACTLOGGER_H
#define GUTIL_ABSTRACTLOGGER_H

#include "gutil_iointerface.h"
#include "gutil_strings.h"
#include "gutil_smartpointer.h"
#include "gutil_flags.h"
#include <time.h>
NAMESPACE_GUTIL;


/** An abstract base implementation of a logging object.
    You must simply inject an object that implements the IOutput into
    the constructor.

    It inherits from SharedData, to enable you to use the SharedSmartPointer
    on any logging subclass.
*/
class AbstractLogger :
        public SharedData
{
    GUTIL_DISABLE_COPY(AbstractLogger);
public:

    /** Enumerates various options that the logger is capable of.
        You can extend this enum and add features as you need to.
        By default, all options are enabled
    */
    enum LoggingOptionsEnum
    {
        /** Logs all messages marked as "info" */
        Option_LogInfo,

        /** Logs all messages marked as "warning" */
        Option_LogWarning,

        /** Logs all messages marked as "error" */
        Option_LogError,

        /** If an exception is logged, this tells us to also log the exception's details  */
        Option_LogExceptionDetails,

        /** If an exception's message is longer than GUTIL_ABSTRACTLOGGER_TRUNCATE_LIMIT
            then we'll truncate it to this limit.
        */
        Option_TruncateExceptionDetails
    };

    /** Declares the flags class, based on the LoggingOptions enum */
    GUTIL_DECLARE_FLAGS(LoggingOptionsFlags, LoggingOptionsEnum);


    /** Describes different severity levels for a log message.

        Different levels of severity will carry a slightly different preamble
        in the logs.  You can customize how this appears by overriding
        prepare_log_message()

        You have the ability to configure the logger to ignore certain
        severity levels, by manipulating the logger options.
    */
    enum MessageLevelEnum
    {
        /** Denotes the "Info" message severity */
        MessageLevel_Info = Option_LogInfo,

        /** Denotes the "Warning" message severity */
        MessageLevel_Warning = Option_LogWarning,

        /** Denotes the "Error" message severity
            \note Exceptions are logged with this severity
        */
        MessageLevel_Error = Option_LogError
    };


    /** Something to be logged.  Contains all the information needed to log an event. */
    struct LoggingData
    {
        String Message;
        String Title;
        MessageLevelEnum MessageLevel;
        time_t LogTime;

        LoggingData()
            :MessageLevel(MessageLevel_Info),
              LogTime(time(NULL))
        {}
    };


    /** If you inject an IO object, this class will take ownership
        of the instance.  If you pass a NULL pointer then it won't
        log anything.

        All available logger options are enabled by default
    */
    AbstractLogger(IOutput *);

    /** You must inject an IO object, and this class will take ownership
        of the instance.  With this constructor you can also initialize the
        options flags
    */
    AbstractLogger(IOutput *, const LoggingOptionsFlags &);

    /** Clears the contents of the log, if applicable.
        The default implementation does nothing.
    */
    virtual void Clear();


    /** Logs a message with the lowest severity */
    void LogInfo(const String &message, const String &title = String())
    {
        LoggingData d;
        d.Message = message;
        d.Title = title;
        d.MessageLevel = MessageLevel_Info;
        this->Log(d);
    }

    /** Logs a warning */
    void LogWarning(const String &message, const String &title = String())
    {
        LoggingData d;
        d.Message = message;
        d.Title = title;
        d.MessageLevel = MessageLevel_Warning;
        this->Log(d);
    }

    /** Logs an error */
    void LogError(const String &message, const String &title = String())
    {
        LoggingData d;
        d.Message = message;
        d.Title = title;
        d.MessageLevel = MessageLevel_Error;
        this->Log(d);
    }

    /** Logs a exception with all of its details.
        It is able to parse the different types of GUtil exceptions.

        If the exception is an ExtendedException, then depending on the logger's
        options, all of the exception data will also be logged,
        including inner exceptions recursively.
    */
    virtual void LogException(const std::exception &);

    /** The base logging function.  Every other log function ends up calling this one.

        If the message makes it through the log filters, then the virtual function
        log_protected() is called to conduct the actual logging.

        You can customize this function on several levels.  Since it is virtual, you may
        override it, or  you can override log_protected() or prepare_log_message(), depending on what you need
        to customize
    */
    virtual void Log(const LoggingData &);

    /** Returns the logging options */
    const LoggingOptionsFlags &Options() const{ return m_options; }

    /** Controls an individual logging option */
    void SetOption(LoggingOptionsEnum opt, bool b){ m_options.SetFlag(opt, b); }

    /** Toggles an individual logging option */
    void ToggleOption(LoggingOptionsEnum opt){ m_options.ToggleFlag(opt); }

    /** Controls the logging options */
    void SetOptions(const LoggingOptionsEnum &o){ m_options = o; }

    /** Overrides the default truncate limit
        \note The truncate option must be enabled for this to have any observable effect
    */
    void SetTruncateLimit(GUINT32 limit){
        m_truncate_limit = limit;
    }

    /** Returns the current truncate limit for the exception messages */
    GUINT32 TruncateLimit() const{ return m_truncate_limit; }

    virtual ~AbstractLogger(){}


protected:

    /** You can customize your own logging format here */
    virtual String prepare_log_message(const LoggingData &);

    /** Defines the filter applied to log messages.  The default implementation
     *  filters based on the message level.
     *  Derived classes can override this to customize filter behavior.
    */
    virtual bool should_log_message(const LoggingData &);

    /** You can customize logging behavior by overriding this function.
        It is called on every log operation, assuming that the operation
        was permitted by the logging options.
    */
    virtual void log_protected(const LoggingData &);


    /** Derived classes may use this accessor to access the io device */
    IOutput *io_device(){ return _io.Data(); }
    /** Derived classes may use this accessor to access the io device */
    IOutput const *io_device() const{ return _io.Data(); }


private:

    SmartPointer<IOutput> _io;
    LoggingOptionsFlags m_options;

    /** An optional character limit on the exception data message. */
    GUINT32 m_truncate_limit;

};


END_NAMESPACE_GUTIL;



GUTIL_DECLARE_FLAG_OPERATORS(GUtil::AbstractLogger::LoggingOptionsFlags,
                             GUtil::AbstractLogger::LoggingOptionsEnum);


#endif // GUTIL_ABSTRACTLOGGER_H
