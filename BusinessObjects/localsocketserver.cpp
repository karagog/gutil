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

#include "localsocketserver.h"
#include "DataAccess/glocalsocketiodevice.h"
using namespace GUtil;
using namespace BusinessObjects;
using namespace DataAccess;

LocalSocketServer::LocalSocketServer(QObject *parent)
    :ServerBase(parent)
{
    connect(&_server, SIGNAL(newConnection()), this, SLOT(new_connection()));
}

LocalSocketServer::~LocalSocketServer()
{
   ShutDownServer();
}

void LocalSocketServer::new_connection()
{
    while(_server.hasPendingConnections())
    {
        GLocalSocketIODevice *sock(new GLocalSocketIODevice(
                _server.nextPendingConnection(), this));

        connect(sock, SIGNAL(Disconnected(QUuid)),
                this, SLOT(client_disconnected(QUuid)));

        iodevice_manager.AddToBundle(sock);

        emit NewConnection(sock->GetIdentity());
    }
}

void LocalSocketServer::ListenForConnections(const QString &identifier,
                                             const QString &modifier)
{
    QString id(QString("%1.%2").arg(identifier).arg(modifier));
    if(!_server.listen(id))
    {
        Core::Exception ex(QString("Cannot listen for connections with id %1")
                           .arg(id).toStdString());
        ex.SetData("error", _server.errorString().toStdString());
        THROW_GUTIL_EXCEPTION(ex);
    }
}

void LocalSocketServer::ShutDownServer()
{
    if(_server.isListening())
    {
        foreach(QUuid id, iodevice_manager.GetIds())
            _socket_device(id).Socket().disconnect();

        _server.close();
    }
}


#endif // NETWORK_FUNCTIONALITY
