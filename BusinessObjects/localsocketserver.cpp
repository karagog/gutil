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

#include "localsocketserver.h"
#include "giodevicebundlemanager.h"
#include "DataAccess/gsocketiodevice.h"
using namespace GUtil;
using namespace BusinessObjects;
using namespace DataAccess;

LocalSocketServer::LocalSocketServer(QObject *parent)
    :QObject(parent),
    _socket_manager(new GIODeviceBundleManager(this))
{
    _server = new QLocalServer(this);

    connect(_server, SIGNAL(newConnection()), this, SLOT(new_connection()));
    connect(_socket_manager, SIGNAL(NewDataArrived(QUuid)),
            this, SLOT(new_data(QUuid)));
}

LocalSocketServer::~LocalSocketServer()
{
    delete _socket_manager;
    delete _server;
}

void LocalSocketServer::new_connection()
{
    while(_server->hasPendingConnections())
    {
        GSocketIODevice *sock(new GSocketIODevice(
                _server->nextPendingConnection(), this));

        connect(sock, SIGNAL(Disconnected(QUuid)),
                this, SLOT(socket_disconnected(QUuid)));

        _socket_manager->AddToBundle(sock);

        emit NewConnection(sock->GetIdentity());
    }
}

void LocalSocketServer::new_data(const QUuid &id)
{
    emit NewMessageArrived(id);
}

void LocalSocketServer::socket_disconnected(const QUuid &id)
{
    emit Disconnected(id);

    _socket_manager->Remove(id);
}

void LocalSocketServer::ListenForConnections(const QString &identifier,
                                             const QString &modifier)
{
    QString id(QString("%1.%2").arg(identifier).arg(modifier));
    if(!_server->listen(id))
    {
        Core::Exception ex(QString("Cannot listen for connections with id %1")
                           .arg(id).toStdString());
        ex.SetData("error", _server->errorString().toStdString());
    }
}

void LocalSocketServer::ShutDownServer()
{
    foreach(QUuid id, _socket_manager->GetIds())
    {
        socket_device(id).Socket().disconnect();
        _socket_manager->Remove(id);
    }
}

void LocalSocketServer::SendMessage(const QByteArray &message,
                                    const QUuid &id)
{
    QList<QUuid> ids;
    if(id.isNull())
        ids = _socket_manager->GetIds();
    else
        ids.append(id);

    foreach(QUuid i, ids)
        _socket_manager->SendData(message, i);
}

QByteArray LocalSocketServer::ReceiveMessage(const QUuid &id)
{
    return _socket_manager->ReceiveData(id);
}

bool LocalSocketServer::HasMessage(const QUuid &id) const
{
   return _socket_manager->HasData(id);
}

GSocketIODevice &LocalSocketServer::socket_device(const QUuid &id)
{
    return (GSocketIODevice &)_socket_manager->Transport(id);
}

const GSocketIODevice &LocalSocketServer::socket_device(const QUuid &id) const
{
    return (const GSocketIODevice &)_socket_manager->Transport(id);
}
