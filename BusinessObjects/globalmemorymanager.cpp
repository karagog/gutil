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
#include <QCoreApplication>
GUTIL_USING_NAMESPACE(BusinessObjects);
GUTIL_USING_NAMESPACE(Logging);
GUTIL_USING_NAMESPACE(Utils);

GlobalMemoryManager *GlobalMemoryManager::m_GlobalInstance(0);

GlobalMemoryManager::GlobalMemoryManager(QCoreApplication *app,
                                         PubSubSystem *pss,
                                         AbstractLogger *logger)
    :QObject(app),
      _p_IsInitialized(true),
      m_pubsub(pss),
      m_logger(logger)
{
    if(pss && logger)
    {
        // Wire up the pubsub system and the logger, if both were given,
        //  so the logger automatically logs exceptions and errors
        connect(pss, SIGNAL(NotifyException(const GUtil::Core::Exception &)),
                logger, SLOT(LogException(const GUtil::Core::Exception &)));
        connect(pss, SIGNAL(NotifyError(QString)),
                logger, SLOT(LogError(QString)));
    }

    connect(app, SIGNAL(aboutToQuit()),
            this, SLOT(ApplicationExiting()));
}

void GlobalMemoryManager::ApplicationExiting()
{
    DeInitialize();
}

void GlobalMemoryManager::DeInitialize()
{
    if(GetIsInitialized())
    {
        // Allow sub-classes to de-initialize first
        application_exiting();

        if(m_pubsub)
        {
            delete m_pubsub;
            m_pubsub = 0;
        }
        if(m_logger)
        {
            delete m_logger;
            m_logger = 0;
        }

        _p_IsInitialized = false;
    }
}

void GlobalMemoryManager::application_exiting()
{
    // Base class does nothing
}
