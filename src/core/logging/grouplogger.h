/*Copyright 2010-2013 George Karagoulis

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

#include "abstractlogger.h"
#include "gutil_vector.h"
#include "gutil_smartpointer.h"
NAMESPACE_GUTIL1(Logging);


/** A logger class that logs to a group of loggers
    \note This class is NOT thread-safe
*/
class GroupLogger :
        public AbstractLogger
{
public:

    inline GroupLogger() :AbstractLogger(0) {}

    /** Overridden from AbstractLogger to log the message to all loggers in the group */
    virtual void Log(const DataObjects::String &message,
                     const DataObjects::String &title = DataObjects::String(),
                     MessageLevelEnum msg_lvl = MessageLevel_Info)
    {
        // Log the message to all of our loggers
        for(DataObjects::Vector< Utils::SharedSmartPointer<AbstractLogger> >::const_iterator iter(m_loggers.begin());
            iter != m_loggers.end(); ++iter)
            (*iter)->Log(message, title, msg_lvl);
    }

    /** Adds a logger to the group.
        This class takes ownership of the memory. There is no way to remove it
        from the group.  Would that ever be useful?
    */
    inline void AddLogger(AbstractLogger *l){ m_loggers.PushBack(l); }


private:

    DataObjects::Vector< Utils::SharedSmartPointer<AbstractLogger> > m_loggers;

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_GROUPLOGGER_H
