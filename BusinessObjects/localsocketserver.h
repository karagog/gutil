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

#ifdef NETWORK_FUNCTIONALITY

#ifndef LOCALSOCKETSERVER_H
#define LOCALSOCKETSERVER_H

#include "connectionmanager.h"
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
        class LocalSocketServer :
                public ConnectionManager
        {
            Q_OBJECT
        public:

            explicit LocalSocketServer(QObject *parent = 0);

            // Throws an exception if listen fails
            void ListenForConnections(const QString &identifier,
                                      const QString &modifier = QString::null);

            // Close all connections and stop listening for new ones
            void ShutDownServer();


        signals:

            void NewConnection(const QUuid &conn_id = QUuid());
            void ClientDisconnected(const QUuid &conn_id = QUuid());


        private slots:

            void new_connection();
            void new_data(const QUuid &, const QByteArray &data);

            void socket_disconnected(const QUuid &);


        private:

            QLocalServer _server;

            inline DataAccess::GSocketIODevice &_socket_device(const QUuid &id){
                return (DataAccess::GSocketIODevice &)iodevice_manager.Transport(id);
            }
            const DataAccess::GSocketIODevice &_socket_device(const QUuid &id) const{
                return (const DataAccess::GSocketIODevice &)iodevice_manager.Transport(id);
            }

        };
    }
}

#endif // LOCALSOCKETSERVER_H

#endif // NETWORK_FUNCTIONALITY
