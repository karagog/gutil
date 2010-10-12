/*Copyright 2010 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef STREAMLOGGER_H
#define STREAMLOGGER_H

#include "Private/abstractlogger.h"

namespace GUtil
{
    namespace Utils
    {
        class PubSubSystem;

        // Logs to an ostream
        class AbstractStreamLogger : public AbstractLogger
        {
            Q_OBJECT
        protected:
            explicit AbstractStreamLogger(std::ostream *, PubSubSystem *pss = 0, QObject *parent = 0);

            virtual void LogMessage_protected(const QString &, MessageLevelEnum);

            // Use this to access the stream
            std::ostream *Stream() const;


        private:
            std::ostream *_stream;
        };
    }
}

#endif // STREAMLOGGER_H
