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

#include "tcpsocketserver.h"
#include "DataAccess/gtcpsocketiodevice.h"
GUTIL_USING_NAMESPACE(BusinessObjects);
GUTIL_USING_NAMESPACE(DataAccess);

TcpSocketServer::TcpSocketServer(QObject *parent)
    :ServerBase(parent)
{
    connect(&_server, SIGNAL(newConnection()), this, SLOT(new_connection()));
}

TcpSocketServer::~TcpSocketServer()
{
    ShutDownServer();
}

void TcpSocketServer::new_connection()
{
    while(_server.hasPendingConnections())
    {
        GTcpSocketIODevice *sock(new GTcpSocketIODevice(
                _server.nextPendingConnection(), this));

        connect(sock, SIGNAL(Disconnected(QUuid)),
                this, SLOT(client_disconnected(QUuid)));

        iodevice_manager.AddToBundle(sock);

        emit NewConnection(sock->GetIdentity());
    }
}

void TcpSocketServer::ListenForConnections(const QHostAddress &address, quint16 port)
{
    if(!_server.listen(address, port))
    {
        Core::Exception ex(QString("Cannot listen for connections on host address %1 and port %2")
                           .arg(address.toString())
                           .arg(port)
                           .toStdString());
        ex.SetData("error", _server.errorString().toStdString());
        THROW_GUTIL_EXCEPTION(ex);
    }
}

void TcpSocketServer::ShutDownServer()
{
    if(_server.isListening())
    {
        foreach(QUuid id, iodevice_manager.GetIds())
            ((DataAccess::GTcpSocketIODevice &)iodevice_manager.Transport(id)).Socket().disconnect();

        _server.close();
    }
}


#endif // NETWORK_FUNCTIONALITY
