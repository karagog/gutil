/*Copyright 2010-2014 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_GROUPLOGGER_H
#define GUTIL_GROUPLOGGER_H

#include "gutil_ilog.h"
#include "gutil_vector.h"
#include <cstdarg>

NAMESPACE_GUTIL;


/** A logger class that logs to a group of loggers.

    It owns all the logs in the group, and will delete them when finished.
*/
class GroupLogger : public ILog
{
    Vector<ILog *> m_loggers;
public:

    /** Constructs a null group logger that doesn't log to anything.
     *  Call AddLogger() to add a logger to the group.
    */
    GroupLogger() {}
    virtual ~GroupLogger(){ for(auto l : m_loggers) delete l; }

    /** Constructs a group logger with the given set of loggers.
     *  \param n The number of loggers
    */
    GroupLogger(int n, ...)
        :m_loggers(n){
        va_list args;
        va_start(args, n);
        for(int i = 0; i < n; ++i)
            AddLogger(va_arg(args, ILog *));
        va_end(args);
    }

    /** Log the message to all loggers in the group. */
    virtual void Log(const LoggingData &d) noexcept{
        for(auto &l : m_loggers) l->Log(d);
    }

    /** Adds a logger to the group.
        This class takes ownership of the memory. There is no way to remove it
        from the group.  Would that ever be useful?
    */
    inline void AddLogger(ILog *l){ m_loggers.PushBack(l); }

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_GROUPLOGGER_H
