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

#ifndef GUTIL_QUEUEDLOGGER_H
#define GUTIL_QUEUEDLOGGER_H

#include "gutil_abstractlogger.h"
#include "gutil_slist.h"
#include "gutil_pair.h"
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

namespace GUtil{ namespace QT{ namespace Logging{


/** A logger that queues logging requests and writes them to
    an actual logger on a background thread.
*/
class QueuedLogger :
        private QThread,
        public ::GUtil::Logging::AbstractLogger
{
    Q_OBJECT


    class _log_item
    {
    public:
        inline _log_item(){}
        inline _log_item(::GUtil::DataObjects::String _s1,
                         ::GUtil::DataObjects::String _s2,
                         MessageLevelEnum _lvl,
                         time_t t)
            :s1(_s1), s2(_s2), lvl(_lvl), tm(t)
        {}

        ::GUtil::DataObjects::String s1, s2;
        MessageLevelEnum lvl;
        time_t tm;
    };


    QMutex m_lock;
    QWaitCondition m_forActivity;
    
    ::GUtil::Utils::SharedSmartPointer< ::GUtil::Logging::AbstractLogger > m_logger;

    // The following variables should be protected by the mutex lock
    ::GUtil::DataObjects::SList< _log_item > m_queue;
    bool m_cancel;
    bool m_flushQueueOnCancel;

public:

    /** Constructs a queued logger.
            You must pass a valid pointer to another logger for this to work,
            and the class will manage the memory for you.
    */
    inline explicit QueuedLogger(::GUtil::Logging::AbstractLogger *l)
        :GUtil::Logging::AbstractLogger(NULL),
          m_logger(l),
          m_cancel(false),
          m_flushQueueOnCancel(true)
    {}

    /** Constructs a queued logger.
            You must pass a valid pointer to another logger for this to work,
            and the class will manage the memory for you.
    */
    inline QueuedLogger(::GUtil::Logging::AbstractLogger *l,
                        const LoggingOptionsFlags &f)
        :GUtil::Logging::AbstractLogger(NULL, f),
          m_logger(l),
          m_cancel(false),
          m_flushQueueOnCancel(true)
    {}

    /** Cancels the thread after completely flushing the queue.
        It will wait for the thread to exit before returning.
    */
    virtual ~QueuedLogger();


    /** Cancels the background thread as soon as possible.

        \param flush_queue Tells the logger to make sure the queue
        is empty before cancelling.  Pass false to abruptly cancel
        the thread, otherwise it will finish writing the remaining
        log messages and quit when it's finished.

        \param block Tells the function to block until all log messages
        are written.  By default it will block, making sure that when
        you exit the application all messages have been written.
    */
    void Cancel(bool flush_queue = true, bool block = true);


protected:

    /** Overridden from QThread, this is the body of the background thread. */
    virtual void run();

    /** Overridden from AbstractLogger, queues a message for
        writing on a background thread.
    */
    virtual void log_protected(const DataObjects::String &,
                               const DataObjects::String &,
                               MessageLevelEnum ml,
                               time_t);
    
};


}}}

#endif // GUTIL_QUEUEDLOGGER_H
