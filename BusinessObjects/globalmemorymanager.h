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

#ifndef GLOBALMEMORYMANAGER_H
#define GLOBALMEMORYMANAGER_H

#include "Utils/pubsubsystem.h"
#include "Logging/abstractlogger.h"
#include "gutil_macros.h"
#include <QObject>

class QCoreApplication;

namespace GUtil{ namespace BusinessObjects{


// A class to encapsulate commonly-used global classes, such as a pub-sub system
//  and a logger.  It can be conveniently subclassed to accommodate custom global
//  memory configurations.  The object is de-allocated automatically when the
//  QApplication terminates.
class GlobalMemoryManager :
        public QObject
{
    Q_OBJECT
public:

    // The globals object attaches itself to the QCoreApplication so it automatically
    //  cleans up the global memory when the application terminates.  It will take
    //  ownership of the pubsub system and logger, if given, and delete them when
    //  ApplicationExiting is called.
    explicit GlobalMemoryManager(QCoreApplication *,
                                 GUtil::Utils::PubSubSystem *pss = 0,
                                 GUtil::Logging::AbstractLogger *logger = 0);


    // Cannot de-initialize in the destructor because we might be subclassed
    //~GlobalMemoryManager();

    void DeInitialize();

    READONLY_PROPERTY( IsInitialized, bool );


    // If you have a different PubSubSystem subclass, you can simply override this
    //  function in a subclass of this global memory object and cast it as your custom type
    inline GUtil::Utils::PubSubSystem *PubSub(){
        return m_pubsub;
    }
    inline GUtil::Logging::AbstractLogger *Logger(){
        return m_logger;
    }


    // For keeping track of the global memory instance
    static inline GlobalMemoryManager *GlobalInstance(){
        return m_GlobalInstance;
    }
    static inline void SetGlobalInstance(GlobalMemoryManager *mm){
        m_GlobalInstance = mm;
    }


public slots:

    void ApplicationExiting();


protected:

    // So derived classes will de-initialize at the appropriate time
    virtual void application_exiting();


private:

    // Here is the global memory for the base class
    GUtil::Utils::PubSubSystem *m_pubsub;
    GUtil::Logging::AbstractLogger *m_logger;

    static GlobalMemoryManager *m_GlobalInstance;

};


}}

#endif // GLOBALMEMORYMANAGER_H
