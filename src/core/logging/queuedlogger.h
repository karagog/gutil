/*Copyright 2014-2015 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_QUEUEDLOGGER_H
#define GUTIL_QUEUEDLOGGER_H

#include <gutil/ilog.h>
#include <gutil/smartpointer.h>
#include <gutil/slist.h>
#include <thread>
#include <mutex>
#include <condition_variable>

NAMESPACE_GUTIL;


/** Logs messages on a background thread for optimal responsiveness.
    
    Logged messages will be put on a queue, which will be processed and logged
    on a background thread. It can log to any ILog, even if it's not thread-safe,
    because this serializes calls to the logging function.
*/
class QueuedLogger : public ILog
{
    GUTIL_DISABLE_COPY(QueuedLogger);
public:

    /** Inject a logging device to do the actual logging.
        This will own and delete the pointer. 
    */
    explicit QueuedLogger(ILog *);
    ~QueuedLogger();

    virtual void Log(const LoggingData &) noexcept;
    
    
private:
    void _worker_thread();
    
    SmartPointer<ILog> m_log;
    SList<LoggingData> m_queue;
    bool m_closing;
    std::mutex m_lock;
    std::condition_variable m_waitCondition;
    std::thread m_worker;
};


END_NAMESPACE_GUTIL;

#endif // GUTIL_QUEUEDLOGGER_H
