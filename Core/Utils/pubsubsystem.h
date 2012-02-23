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

#ifndef GUTIL_PUBSUBSYSTEM_H
#define GUTIL_PUBSUBSYSTEM_H

#include "Core/macros.h"
#include "Core/DataObjects/vector.h"

namespace GUtil{

template<bool>class Exception;


namespace Utils{


/** Declares a notification topic, to which classes can subscribe.

    You subscribe by calling "Subscribe<name>Handler()".
    You notify by calling Notify<name>Handler()

    \param name The name of the event.  This name defines the name of
    the handler class for the event, which becomes class <name>Handler.
*/
#define DECLARE_TOPIC(name) \
public: \
    class name##Handler{ \
    public:  virtual void Handle##name() = 0; \
    }; \
private: \
    GUtil::DataObjects::Vector<name##Handler *> m_##name##_handlers; \
public: \
    void Subscribe##name##Handler(name##Handler *h){ m_##name##_handlers.PushBack(h); } \
    void UnSubscribe##name##Handler(name##Handler *h){ m_##name##_handlers.RemoveAll(h); } \
    void Publish##name(){ for(GUINT32 i(0); i < m_##name##_handlers.Length(); ++i) m_##name##_handlers[i]->Handle##name(); }



/** Declares a notification topic, to which classes can subscribe.

    You subscribe by calling "Subscribe<name>Handler()".
    You notify by calling Notify<name>Handler()

    \param name The name of the event.  This name defines the name of
    the handler class for the event, which becomes class <name>Handler.
    \param type The type of an argument passed with the event.  You should
    use const references whenever possible.
*/
#define DECLARE_TOPIC2(name, type) \
public: \
    class name##Handler{ \
    public:  virtual void Handle##name(type) = 0; \
    }; \
private: \
    GUtil::DataObjects::Vector<name##Handler *> m_##name##_handlers; \
public: \
    void Subscribe##name##Handler(name##Handler *h){ m_##name##_handlers.PushBack(h); } \
    void UnSubscribe##name##Handler(name##Handler *h){ m_##name##_handlers.RemoveAll(h); } \
    void Publish##name(type d){ for(GUINT32 i(0); i < m_##name##_handlers.Length(); ++i) m_##name##_handlers[i]->Handle##name(d); }



/** Implements a message broker between layers of your software.

    The publish/subscription model is one where notifications are sent and
    received without knowing who you're talking to.  Your class
    can subscribe to a notification by implementing the appropriate handler interface,
    and then its method will be called whenever someone notifies of said event.

    This pub/sub model allows you to safely propagate messages up through the
    software layers, where normally this would violate the layered model.  In
    a layered software architecture, a pub/sub system is represented off to the
    side, existing at all levels of the hierarchy.
*/
class PubSubSystem
{
public:

    /** Describes a message that can be subscribed to, which reports
        progress of a specific job id.
    */
    class ProgressMessage
    {
    public:

        /** The amount of progress achieved so far. */
        int Progress;
        /** The id of the job creating progress.  This will depend on your own
            implementation, but you can track the progress of several jobs using
            this identifier.
        */
        int ProgressId;

        inline ProgressMessage() :Progress(-1), ProgressId(-1){}
        inline ProgressMessage(int progress, int progress_id = -1)
            :Progress(progress), ProgressId(progress_id){}

    };

    DECLARE_TOPIC2(ProgressUpdated, const ProgressMessage &)

};



}}

#endif // GUTIL_PUBSUBSYSTEM_H
