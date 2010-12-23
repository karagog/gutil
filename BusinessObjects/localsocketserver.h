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

#ifndef LOCALSOCKETSERVER_H
#define LOCALSOCKETSERVER_H

#include <QObject>
#include <QString>
#include <QUuid>
#include <QLocalServer>

namespace GUtil
{
    namespace DataAccess
    {
        class GSocketIODevice;
    }

    namespace BusinessObjects
    {
        class GIODeviceBundleManager;

        class LocalSocketServer :
                public QObject
        {
            Q_OBJECT
        signals:

            void NewConnection(const QUuid &);

            void NewMessageArrived(const QUuid &);

            void Disconnected(const QUuid &);


        public:

            explicit LocalSocketServer(QObject *parent = 0);
            ~LocalSocketServer();

            // Throws an exception if listen fails
            void ListenForConnections(const QString &identifier,
                                      const QString &modifier = QString::null);

            // Close all connections and stop listening for new ones
            void ShutDownServer();

            // If you leave 'id' blank, it will broadcast to all connections
            void SendMessage(const QByteArray &message,
                             const QUuid &id = QUuid());

            // Returns a null byte array if nothing to read
            QByteArray ReceiveMessage(const QUuid &);
            bool HasMessage(const QUuid &) const;


        private slots:

            void new_connection();
            void new_data(const QUuid &);

            void socket_disconnected(const QUuid &);


        private:

            GIODeviceBundleManager *_socket_manager;
            QLocalServer *_server;

            DataAccess::GSocketIODevice &socket_device(const QUuid &);
            const DataAccess::GSocketIODevice &socket_device(const QUuid &) const;

        };
    }
}

#endif // LOCALSOCKETSERVER_H
