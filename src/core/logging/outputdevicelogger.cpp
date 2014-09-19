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

#include "outputdevicelogger.h"
#include "gutil_iio.h"

NAMESPACE_GUTIL;


OutputDeviceLogger::~OutputDeviceLogger() {}

String OutputDeviceLogger::prepare_log_text(const LoggingData &d) noexcept
{
    char const *msg_id;
    switch(d.MessageLevel)
    {
    case LogLevelInfo:
        msg_id = "Info";
        break;
    case LogLevelWarning:
        msg_id = "Warning";
        break;
    case LogLevelError:
        msg_id = "ERROR";
        break;
    default:
        GASSERT(false);
        msg_id = "(unknown)";
        break;
    }

    char tm_buf[30] = {'I', '\0'};
    strftime(tm_buf, 30, "%Y-%m-%d %H:%M:%S", localtime(&d.LogTime));

    return String::Format("%s  %s: %s%s\n\n\n",
                          tm_buf, msg_id,
                          d.Title.Length() == 0 ? "" : d.Title.ConstData(),
                          d.Message.Length() == 0 ? "" : String(1 + d.Message.Length()).Append("\n").Append(d.Message).ConstData());
}

void OutputDeviceLogger::log_text_out(const String &msg) noexcept
{
    try{
        m_out->WriteBytes((byte const *)msg.ConstData(), msg.Length());
    } catch(...) {
        GDEBUG("I just trapped an exception while logging...");
    }
}


END_NAMESPACE_GUTIL;
