/*Copyright 2011 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "globalmemorymanager.h"
#include "gutil_macros.h"
#include "Core/extendedexception.h"
GUTIL_USING_NAMESPACE(Utils);

QHash<QString, void*> GlobalMemoryManager::m_GlobalInstances;
QReadWriteLock GlobalMemoryManager::m_GlobalInstancesLock;

void GlobalMemoryManager::set_global_instance(const QString &k, void *v)
{
    m_GlobalInstancesLock.lockForWrite();
    try
    {
        if(m_GlobalInstances.contains(k))
            THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                       QString("Global instance '%1' already set")
                                       .arg(k).toAscii().constData());
        m_GlobalInstances.insert(k, v);
    }
    catch(...)
    {
        m_GlobalInstancesLock.unlock();
        throw;
    }
    m_GlobalInstancesLock.unlock();
}

void *GlobalMemoryManager::get_global_instance(const QString &k)
{
    void *ret;
    m_GlobalInstancesLock.lockForRead();
    try
    {
        if(m_GlobalInstances.contains(k))
            ret = m_GlobalInstances.value(k);
        else
            THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                       QString("Global instance '%1' not set")
                                       .arg(k).toAscii().constData());
    }
    catch(...)
    {
        m_GlobalInstancesLock.unlock();
        throw;
    }
    m_GlobalInstancesLock.unlock();
    return ret;
}
