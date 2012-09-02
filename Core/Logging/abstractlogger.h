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

#ifndef GUTIL_ABSTRACTLOGGER_H
#define GUTIL_ABSTRACTLOGGER_H

#include "gutil_iointerface.h"
#include "gutil_strings.h"
#include "gutil_smartpointer.h"
#include "gutil_flags.h"
#include <time.h>
NAMESPACE_GUTIL1(Logging);


/** An abstract base implementation of a logging object.
    You must simply inject an object that implements the OutputInterface into
    the constructor.

    It inherits from SharedData, to enable you to use the SharedSmartPointer
    on any logging subclass.
*/
class AbstractLogger :
        public Utils::SharedData
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


    /** You must inject an IO object, and this class will take ownership
        of the instance.

        All available logger options are enabled by default
    */
    explicit AbstractLogger(DataAccess::OutputInterface *);

    /** You must inject an IO object, and this class will take ownership
        of the instance.  With this constructor you can also initialize the
        options flags
    */
    AbstractLogger(DataAccess::OutputInterface *, const LoggingOptionsFlags &);

    /** Clears the contents of the log, if applicable.
        The default implementation does nothing.
    */
    virtual void Clear();


    /** Logs a message with the lowest severity */
    inline void LogInfo(const DataObjects::String &message, const DataObjects::String &title){
        Log(message, title, MessageLevel_Info);
    }

    /** Logs a warning */
    inline void LogWarning(const DataObjects::String &message, const DataObjects::String &title){
        Log(message, title, MessageLevel_Warning);
    }

    /** Logs an error */
    inline void LogError(const DataObjects::String &message, const DataObjects::String &title){
        Log(message, title, MessageLevel_Error);
    }

    /** Logs a GUtil exception with all of its details.
        If the exception is an ExtendedException, then depending on the logger's
        options, all of the exception data will also be logged,
        including inner exceptions recursively.
    */
    void LogException(const GUtil::Exception<false> &);

    /** The base logging function.  Every other log function ends up calling this one.

        If the message makes it through the log filters, then the virtual function
        log_protected() is called to conduct the actual logging.

        If you want to customize logging behavior, you can either override
        log_protected() or prepare_log_message(), depending on what you need
        to customize
    */
    inline void Log(const DataObjects::String &message,
                    const DataObjects::String &title = DataObjects::String(),
                    MessageLevelEnum ml = MessageLevel_Info,
                    time_t current_time = time(NULL))
    {
        if(should_log_message(ml))
            log_protected(message, title, ml, current_time);
    }

    /** Returns the logging options */
    inline const LoggingOptionsFlags &Options() const{ return m_options; }

    /** Controls an individual logging option */
    inline void SetOption(LoggingOptionsEnum opt, bool b){ m_options.SetFlag(opt, b); }

    /** Toggles an individual logging option */
    inline void ToggleOption(LoggingOptionsEnum opt){ m_options.ToggleFlag(opt); }

    /** Controls the logging options */
    inline void SetOptions(const LoggingOptionsEnum &o){ m_options = o; }

    /** Overrides the default truncate limit
        \note The truncate option must be enabled for this to have any observable effect
    */
    inline void SetTruncateLimit(GUINT32 limit){
        m_truncate_limit = limit;
    }

    /** Returns the current truncate limit for the exception messages */
    inline GUINT32 TruncateLimit() const{ return m_truncate_limit; }

    virtual ~AbstractLogger(){}


protected:

    /** You can customize your own logging format here */
    virtual DataObjects::String prepare_log_message(const DataObjects::String &,
                                                    const DataObjects::String &,
                                                    MessageLevelEnum ml,
                                                    time_t);

    /** You can customize logging behavior by overriding this function.
        It is called on every log operation, assuming that the operation
        was permitted by the logging options.
    */
    virtual void log_protected(const DataObjects::String &,
                               const DataObjects::String &,
                               MessageLevelEnum ml,
                               time_t);

    /** Derived classes may use this accessor to access the io device */
    inline DataAccess::OutputInterface *io_device(){ return _io.Data(); }
    /** Derived classes may use this accessor to access the io device */
    inline DataAccess::OutputInterface const *io_device() const{ return _io.ConstData(); }

    /** Based solely on the message severity, should we log this message?
        Derived classes can use this function to inherit the same logic
        for which messages to log, and which to ignore.
    */
    inline bool should_log_message(MessageLevelEnum ml) const{
        return m_options.TestFlag((LoggingOptionsEnum)ml);
    }


private:

    Utils::SmartPointer<DataAccess::OutputInterface> _io;
    LoggingOptionsFlags m_options;

    /** An optional character limit on the exception data message. */
    GUINT32 m_truncate_limit;

};


END_NAMESPACE_GUTIL1;



GUTIL_DECLARE_FLAG_OPERATORS(GUtil::Logging::AbstractLogger::LoggingOptionsFlags,
                             GUtil::Logging::AbstractLogger::LoggingOptionsEnum);


#endif // GUTIL_ABSTRACTLOGGER_H
