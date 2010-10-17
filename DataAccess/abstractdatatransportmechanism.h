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

#ifndef ITRANSPORTMECHANISM_H
#define ITRANSPORTMECHANISM_H

#include <QByteArray>
#include <QMutex>
#include <QObject>
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

        class AbstractDataTransportMechanism : public QObject, public Core::Interfaces::IReadOnlyObject
        {
            Q_OBJECT

        public slots:

            void SendData(const QByteArray &);
            void Write(const QByteArray &);


        public:

            // Note: You can rely on the signal 'notifyNewData' to get the new data, but you can also call this manually
            QByteArray ReceiveData();

            enum StateEnum
            {
                GoodState,
                ErrorState
            };

            AbstractDataTransportMechanism &operator << (const char*);
            AbstractDataTransportMechanism &operator << (const std::string &);
            AbstractDataTransportMechanism &operator << (const QString &);
            void operator >> (QByteArray &);
            void operator >> (QString &);
            void operator >> (std::string &);

        signals:

            // Tell the world about the new data that arrived asynchronously
            void notifyNewData(const QByteArray &);

            void notifyDataSent(const QByteArray &);


        protected:

            explicit AbstractDataTransportMechanism(QObject *parent = 0);

            // Derived classes must implement these functions
            //  Note that locking is taken care of by this interface class,
            //  so you can trust that these are atomic WRT each other
            virtual void send_data(const QByteArray&) throw(GUtil::Core::DataTransportException) = 0;
            virtual QByteArray receive_data()
                    throw(GUtil::Core::DataTransportException,
                          GUtil::Core::EndOfFileException) = 0;


            // A reference to the last data received
            QByteArray last_data_received;


            // You must derive from this function to program special logic
            //   to determine if there's data available.  This will depend
            //   on the method of transport you're using.  Set the boolean
            //   to true if there is data available.
            virtual void update_has_data_variable(bool &has_data_variable)
                    throw(GUtil::Core::DataTransportException) = 0;

            virtual QString ReadonlyMessageIdentifier();


        protected slots:

            // Connect this to an update signal, or call it manually to force an update
            void trigger_update_has_data_available();


        private:

            // Protects us so we can be re-entrant when using the transport
            QMutex _lock;

            // Note: Only use 'has_data' and 'set_has_data' to change this boolean
            bool _has_data;

            StateEnum _cur_state;

        };
    }
}

#endif // ITRANSPORTMECHANISM_H
