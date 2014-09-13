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

NAMESPACE_GUTIL1(QT);


QueuedLogger::~QueuedLogger()
{
    // Cancel the thread and flush the queue
    Cancel(true, true);
}

void QueuedLogger::run()
{
    m_lock.lock();
    Forever([this]{
        // Critical Section
        LoggingData tmp_item;
        {
            while(!m_cancel && m_queue.IsEmpty())
                m_forActivity.wait(&m_lock);

            if(m_cancel && (m_queue.IsEmpty() || !m_flushQueueOnCancel))
                return false;

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
        m_logger->Log(tmp_item);

        // Pick up the lock again before continuing the loop
        m_lock.lock();
        return true;
    });
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

void QueuedLogger::log_protected(const LoggingData &d)
{
    m_lock.lock();
    {
        // Push the job on the queue
        m_queue.PushBack(d);

        // Wake up the background thread 'cause there's work to do
        m_forActivity.wakeOne();
    }
    m_lock.unlock();


    // Start the thread if it's not already running
    if(!isRunning())
        start();
}


END_NAMESPACE_GUTIL1;
