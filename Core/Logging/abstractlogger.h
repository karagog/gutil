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

NAMESPACE_GUTIL1(Logging);


/** An abstract base implementation of a logging object.
    You must simply inject an object that implements the OutputInterface into
    the constructor.
*/
class AbstractLogger :
        public Utils::SharedData
{
    GUTIL_DISABLE_COPY(AbstractLogger);
public:

    /** You must inject an IO object, and this class will take ownership
        of the instance.
    */
    explicit AbstractLogger(DataAccess::OutputInterface *);

    /** Describes different levels of logging importance.
        These will affect how the message appears in the logs.
    */
    enum MessageLevelEnum
    {
        Info = 0,
        Warning = 1,
        Error = 2
    };


    /** The base logging function.  Every other log function ends up calling
        this in the end, so subclasses can override it to customize logging
        behavior.
    */
    virtual void Log(const DataObjects::String &message,
                     const DataObjects::String &title = DataObjects::String(),
                     MessageLevelEnum ml = Info);

    /** Logs a GUtil exception with all of its details.
        If the exception is an ExtendedException, then all of the
        exception data will also be logged, including inner exceptions
        recursively.
    */
    void LogException(const GUtil::Exception<false> &);

    /** Controls whether or not to log the internal data of an exception */
    void LogExceptionDetails(bool);

    /** Determines the level under which we won't log anything */
    void SetMessageLevelTolerance(MessageLevelEnum);
    /** The level under which we won't log anything. */
    MessageLevelEnum MessageLevelTolerance();

    virtual ~AbstractLogger(){}


protected:

    /** You can customize your own logging format here */
    virtual DataObjects::String prepare_log_message(const DataObjects::String &,
                                                    const DataObjects::String &,
                                                    MessageLevelEnum ml = Info);

    /** Derived classes may use this accessor to access the io device */
    inline DataAccess::OutputInterface *io_device(){ return _io.Data(); }
    /** Derived classes may use this accessor to access the io device */
    inline DataAccess::OutputInterface const *io_device() const{ return _io.ConstData(); }


private:

    Utils::SmartPointer<DataAccess::OutputInterface> _io;
    MessageLevelEnum m_message_level_tolerance;
    bool m_logExceptionDetails;

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_ABSTRACTLOGGER_H
