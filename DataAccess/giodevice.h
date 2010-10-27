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

#ifndef GIODEVICE_H
#define GIODEVICE_H

#include <QByteArray>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>
#include "Core/exception.h"
#include "Core/Interfaces/ireadonlyobject.h"

namespace GUtil
{
    namespace DataAccess
    {
        // This is an abstract base class to represent different methods of data transport

        // The interface to the outside world is limited to two functions:
        //   sendData(QByteArray) - SLOT
        //   notifyNewData(QByteArray) - SIGNAL

        // These functions simplify data transportation to send/receive commands.  The derived transport class
        //   will be responsible for the actual transport mechanism, including protocols and the data exchange.

        class GIODevice : public QThread, public Core::Interfaces::IReadOnlyObject
        {
            Q_OBJECT

        public slots:
            void SendData(const QByteArray &);
            void Write(const QByteArray &);

        signals:
            // This signal happens when there's new data available
            void ReadyRead();

        public:
            virtual ~GIODevice();

            // Note: You can rely on the signal 'ReadyRead' to get the new data, but you can also call this manually
            QByteArray ReceiveData(bool block = true)
                    throw(Core::DataTransportException);

            virtual bool HasDataAvailable() = 0;

            GIODevice &operator << (const char*);
            GIODevice &operator << (const std::string &);
            GIODevice &operator << (const QString &);
            void operator >> (QByteArray &);
            void operator >> (QString &);
            void operator >> (std::string &);

        protected:
            explicit GIODevice(QObject *parent = 0);

            // Because we're a QThread, we can implement this function to run when 'start()' is called
            virtual void run();

            // Derived classes must implement these functions
            //  Note that locking is taken care of by this interface class,
            //  so you can trust that these are atomic WRT each other
            virtual void send_data(const QByteArray&)
                    throw(GUtil::Core::DataTransportException) = 0;
            virtual QByteArray receive_data()
                    throw(GUtil::Core::DataTransportException) = 0;

            virtual QString ReadonlyMessageIdentifier();

            // Derived classes can wait on this condition to block until
            //   new data is available
            QWaitCondition condition_new_data_available;

        protected slots:
            // This emits the readyRead signal
            void raiseReadyRead();

        private:
            // Protects us so we can be thread-safe
            QMutex this_giodevice_lock;

        };
    }
}

#endif // GIODEVICE_H