/*Copyright 2012 George Karagoulis
  
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

NAMESPACE_GUTIL2(QT, Logging);


QueuedLogger::~QueuedLogger()
{
    // Cancel the thread and flush the queue
    Cancel(true, true);
}

void QueuedLogger::run()
{
    _log_item tmp_item;

    m_lock.lock();

    G_FOREVER
    {
        // Critical Section
        {
            while(!m_cancel && m_queue.IsEmpty())
                m_forActivity.wait(&m_lock);

            if(m_cancel && (m_queue.IsEmpty() || !m_flushQueueOnCancel))
                break;

            // We must be sure that we never go past this point with an empty
            //  queue, otherwise we crash
            GASSERT(!m_queue.IsEmpty());

            // Grab the next message at the front of the queue
            tmp_item = m_queue.Front();
            m_queue.PopFront();
        }
        // Relinquish the lock while we write the data.  This allows
        //  someone to queue while we're busy writing to the output device.
        m_lock.unlock();

        // Call the base logger implementation to actually log the data
        AbstractLogger::log_protected(tmp_item.s1, tmp_item.s2, tmp_item.lvl);

        // Pick up the lock again before continuing the loop
        m_lock.lock();
    }

    m_lock.unlock();
}

void QueuedLogger::Cancel(bool flush_queue, bool block)
{
    m_lock.lock();
    {
        // Pass on the memo that we want to cancel
        m_cancel = true;
        m_flushQueueOnCancel = flush_queue;

        // Wake up the background thread
        m_forActivity.wakeOne();
    }
    m_lock.unlock();


    // Optionally wait for the thread to finish writing
    if(block)
        wait();
}

void QueuedLogger::log_protected(const DataObjects::String &s1,
                                 const DataObjects::String &s2,
                                 MessageLevelEnum ml)
{
    m_lock.lock();
    {
        // Push the job on the queue
        m_queue.PushBack(_log_item(s1, s2, ml));

        // Wake up the background thread 'cause there's work to do
        m_forActivity.wakeOne();
    }
    m_lock.unlock();


    // Start the thread if it's not already running
    if(!isRunning())
        start();
}


END_NAMESPACE_GUTIL2;
