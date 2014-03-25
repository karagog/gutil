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

#include "threadsafelogger.h"

NAMESPACE_GUTIL1(QT);


ThreadSafeLogger::ThreadSafeLogger(AbstractLogger *al, bool auto_delete)
    :AbstractLogger(NULL),
      m_logger(al),
      m_autoDelete(auto_delete)
{
}

ThreadSafeLogger::~ThreadSafeLogger()
{
    if(m_autoDelete)
        delete m_logger;
}

void ThreadSafeLogger::Log(const LoggingData &d)
{
    m_mutex.lock();
    m_logger->Log(d);
    m_mutex.unlock();
}


END_NAMESPACE_GUTIL1;
