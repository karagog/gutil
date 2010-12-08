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

#ifndef CONSOLETRANSPORT_H
#define CONSOLETRANSPORT_H

#include "giodevice.h"
#include <QMutex>
#include <QQueue>

namespace GUtil
{
    namespace DataAccess
    {
        class GConsoleIODevice : public GIODevice
        {
            Q_OBJECT
        public:
            explicit GConsoleIODevice(QObject *parent = 0);
            virtual ~GConsoleIODevice();

            inline void Engage(){ SetEngaged(true); }
            inline void Disengage(){ SetEngaged(false); }

            inline bool IsEngaged() const{ return _engaged; }

            virtual bool HasDataAvailable() const;

        public slots:
            void WriteLine(const QString &);
            QString ReadLine(bool block = true);

            // stops/starts the object from listening to cin/cout
            void SetEngaged(bool);

        protected:
            // Just reads/writes to stdin/out
            virtual void send_data(const QByteArray &)
                    throw(GUtil::Core::DataTransportException);
            virtual QByteArray receive_data()
                    throw(GUtil::Core::DataTransportException);

            // We continually read cin on a separate thread
            virtual void run();

        private:

            QQueue<QString> _messages_received;

            // Only one of these objects can interface with the console
            static QMutex global_console_mutex;

            bool _engaged;
            void _fail_if_not_initialized();
        };
    }
}

#endif // CONSOLETRANSPORT_H
