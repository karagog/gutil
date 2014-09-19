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

#include "globallogger.h"
#include "queuedlogger.h"
#include "gutil_smartpointer.h"

NAMESPACE_GUTIL;


// The instance of the global logger that was set with SetGlobalLogger()
static ILog *__globallog_instance = NULL;

// forwards all calls to the global logger instance, ignoring them if there is no logger set.
struct global_logger_t : public ILog{
    virtual void Log(const LoggingData &d) noexcept{
        if(__globallog_instance)
            __globallog_instance->Log(d);
    }
    virtual void LogException(const std::exception &e) noexcept{
        if(__globallog_instance)
            __globallog_instance->LogException(e);
    }
    virtual void Clear(){
        if(__globallog_instance)
            __globallog_instance->Clear();
    }
    ~global_logger_t(){ GDEBUG2(__globallog_instance,
                                "WARNING: You forgot to clean up the global logger!"); }
} static __globallogger;

ILog &GlobalLogger()
{
    return __globallogger;
}

void SetGlobalLogger(ILog *l)
{
    GDEBUG2(NULL != dynamic_cast<QueuedLogger*>(l),
            "Do not set a QueuedLogger as the global logger; I'm already doing that for you!");

    // Destroy the old logger
    delete __globallog_instance;

    // Set the new logger
    if(l) __globallog_instance = new QueuedLogger(l);
    else  __globallog_instance = NULL;
}


END_NAMESPACE_GUTIL;
