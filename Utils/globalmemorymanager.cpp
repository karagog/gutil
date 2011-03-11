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
#include "Core/exception.h"
GUTIL_USING_NAMESPACE(Utils);

GlobalMemoryManager *GlobalMemoryManager::m_GlobalInstance(0);

void GlobalMemoryManager::SetGlobalInstance(GlobalMemoryManager *mm)
{
    if(m_GlobalInstance)
        THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                   "Global instance already set");

    m_GlobalInstance = mm;
}

GlobalMemoryManager *GlobalMemoryManager::GlobalInstance()
{
    return m_GlobalInstance;
}
