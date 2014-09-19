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

#include "queuedlogger.h"
using namespace std;

NAMESPACE_GUTIL;


QueuedLogger::QueuedLogger(ILog *l)
    :m_log(l),
      m_closing(false),
      m_worker(&QueuedLogger::_worker_thread, this)    // Start the worker
{}

QueuedLogger::~QueuedLogger()
{
    m_lock.lock();
    m_closing = true;
    m_lock.unlock();
    m_waitCondition.notify_one();
    m_worker.join();
}

void QueuedLogger::Log(const LoggingData &d) noexcept
{
    lock_guard<mutex> lkr(m_lock);
    m_queue.PushBack(d);
    m_waitCondition.notify_one();
}

void QueuedLogger::_worker_thread()
{
    unique_lock<mutex> lkr(m_lock);
    while(!m_closing){
        // Wait for something to do
        m_waitCondition.wait(lkr, [&]{ return m_closing || !m_queue.IsEmpty(); });
        
        // Process the queue, even if we're closing
        while(!m_queue.IsEmpty()){
            // Grab the next item from the queue (a copy because we're going to release the lock)
            const LoggingData d(m_queue.Front());
            m_queue.PopFront();
            
            // Release the lock while we log
            lkr.unlock();
            m_log->Log(d);  // This is noexcept, so don't bother catching
            lkr.lock();
        }
    }
}


END_NAMESPACE_GUTIL;
