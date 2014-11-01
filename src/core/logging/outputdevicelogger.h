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

#ifndef GUTIL_OUTPUTDEVICELOGGER_H
#define GUTIL_OUTPUTDEVICELOGGER_H

#include <gutil/ilog.h>
#include <gutil/smartpointer.h>

NAMESPACE_GUTIL;
class IOutput;


/** Logs to a generic output device. Simply inject an IOutput
 *  to the constructor and this will log all messages to it.
 *
 *  This will take care to delete the output device.
*/
class OutputDeviceLogger : public ILog
{
    SmartPointer<IOutput> m_out;
public:

    inline OutputDeviceLogger(IOutput *o) :m_out(o) {}
    ~OutputDeviceLogger();

    /** If the message makes it through the log filters, then the virtual function
        log() is called to conduct the actual logging. The textual message is generated
        by calling prepare_log_message(), which you can override.
    */
    virtual void Log(const LoggingData &d) noexcept{
        if(should_log(d))
            log_text_out(prepare_log_text(d));
    }

protected:

    /** You can customize your own logging format here */
    virtual String prepare_log_text(const LoggingData &) noexcept;

    /** The low level logging function, writes the log text to the output device. */
    virtual void log_text_out(const String &) noexcept;
};


END_NAMESPACE_GUTIL;

#endif // GUTIL_OUTPUTDEVICELOGGER_H
