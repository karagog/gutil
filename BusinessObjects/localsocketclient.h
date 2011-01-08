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

#ifndef LOCALSOCKETCLIENT_H
#define LOCALSOCKETCLIENT_H

#include "BusinessObjects/giodevicebundlemanager.h"
#include <QObject>
#include <QUuid>

namespace GUtil
{
    namespace BusinessObjects
    {
        class LocalSocketClient :
                public QObject
        {
            Q_OBJECT
        signals:

            void NewMessageArrived(const QUuid &message_id = QUuid());
            void Disconnected();


        public:

            explicit LocalSocketClient(QObject *parent = 0);

            void ConnectToServer(const QString &identifier,
                                 const QString &modifier = QString::null);
            void DisconnectFromServer();
            bool IsConnected() const;

            // You can send a generic message, or reply specifically to a
            //  message you received
            void SendMessage(const QByteArray &);
            void Reply(const QUuid &message_id, const QByteArray &);

            QByteArray ReceiveMessage();
            bool HasMessage() const;

            inline int MaxThreads() const{
                return _socket_manager.MaxThreads();
            }


        private slots:

            void _socket_disconnected();
            void _socket_new_data(const QUuid &, const QByteArray &);


        private:

            GIODeviceBundleManager _socket_manager;

        };
    }
}

#endif // LOCALSOCKETCLIENT_H
