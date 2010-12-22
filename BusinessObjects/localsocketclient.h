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

#include <QObject>
#include <QUuid>

namespace GUtil
{
    namespace BusinessObjects
    {
        class GIODeviceBundleManager;

        class LocalSocketClient :
                public QObject
        {
            Q_OBJECT
        signals:

            void NewMessageArrived();
            void Disconnected();


        public:

            explicit LocalSocketClient(QObject *parent = 0);
            ~LocalSocketClient();

            void ConnectToServer(const QString &identifier,
                                 const QString &modifier = QString::null);
            void Disconnect();
            bool IsConnected() const;

            void SendMessage(const QByteArray &);
            QByteArray ReceiveMessage();
            bool HasMessage() const;


        private slots:

            void _socket_disconnected();
            void _socket_new_data();


        private:

            GIODeviceBundleManager *_socket_manager;

        };
    }
}

#endif // LOCALSOCKETCLIENT_H
