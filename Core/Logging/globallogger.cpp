/*Copyright 2010-2012 George Karagoulis

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
USING_NAMESPACE_GUTIL1(Utils);
USING_NAMESPACE_GUTIL1(DataObjects);

NAMESPACE_GUTIL1(Logging);

// Static variables
Map<int, SharedSmartPointer<AbstractLogger> > GlobalLogger::m_loggers;
int GlobalLogger::m_default_logger_id = 0;


void GlobalLogger::_translate_logger_id(int &id, bool allow_new_id)
{
    if(id == DebugId);
    else if(id == DefaultId)
        id = m_default_logger_id;
    else if(id == NewId)
    {
        if(!allow_new_id)
            THROW_NEW_GUTIL_EXCEPTION2(ArgumentException, "Can't create new ID here");

        // Auto-assign a logger id
        id = 1;
        while(m_loggers.Contains(id))
            id++;
    }
    else if(id < 0)
    {
        THROW_NEW_GUTIL_EXCEPTION2(ArgumentException, "Logger ID not recognized");
    }
}

END_NAMESPACE_GUTIL1;
