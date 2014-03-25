/*Copyright 2013 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_THREADSAFELOGGER_H
#define GUTIL_THREADSAFELOGGER_H

#include "gutil_abstractlogger.h"
#include <QMutex>

NAMESPACE_GUTIL1(QT);


/** A logger that is safe to use across threads.
 *  It serializes the calls to the log functions with a QMutex, thus allowing only one thread to use it at a time.
 *  You must provide an actual logger implementation through the constructor.
*/
class ThreadSafeLogger :
        public GUtil::AbstractLogger
{
    GUtil::AbstractLogger *m_logger;
    QMutex m_mutex;
    bool m_autoDelete;
public:

    /** Constructs a thread-safe logger.  You must provide a logging implementation.
     *  \param auto_delete Controls whether this will delete the logging implementation at the end.
    */
    ThreadSafeLogger(GUtil::AbstractLogger *, bool auto_delete = true);
    virtual ~ThreadSafeLogger();

    /** All logging requests go through this function, which will require a lock to actually use the log
    */
    virtual void Log(const LoggingData &d);

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_THREADSAFELOGGER_H
