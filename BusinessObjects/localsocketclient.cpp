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

#include "localsocketclient.h"
#include "giodevicebundlemanager.h"
#include "DataAccess/gsocketiodevice.h"
using namespace GUtil;
using namespace DataAccess;
using namespace BusinessObjects;

LocalSocketClient::LocalSocketClient(QObject *parent)
    :QObject(parent),
    _socket_manager(new GIODeviceBundleManager(this))
{
    connect(_socket_manager, SIGNAL(NewDataArrived()),
            this, SLOT(_socket_new_data()));
}

LocalSocketClient::~LocalSocketClient()
{
    delete _socket_manager;
}

void LocalSocketClient::ConnectToServer(const QString &identifier,
                                        const QString &modifier)
{
    GSocketIODevice *sock(new GSocketIODevice(new QLocalSocket, this));

    QString id(QString("%1.%2").arg(identifier).arg(modifier));
    sock->Socket().connectToServer(id);
    if(sock->Socket().waitForConnected(5000))
    {
        connect(&sock->Socket(), SIGNAL(disconnected()),
                this, SLOT(_socket_disconnected()));
        _socket_manager->AddToBundle(sock);
    }
    else
    {
        Core::Exception ex(QString("Unable to connect to server id %1")
                           .arg(id).toStdString());
        ex.SetData("error", sock->Socket().errorString().toStdString());
        delete sock;
        THROW_GUTIL_EXCEPTION(ex);
    }
}

void LocalSocketClient::Disconnect()
{
    ((GSocketIODevice &)_socket_manager->Transport())
            .Socket().disconnectFromServer();

    _socket_manager->RemoveAll();
}

bool LocalSocketClient::IsConnected() const
{
    return _socket_manager->GetIds().count() > 0;
}

void LocalSocketClient::_socket_disconnected()
{
    emit Disconnected();
}

void LocalSocketClient::_socket_new_data()
{
    emit NewMessageArrived();
}

void LocalSocketClient::SendMessage(const QByteArray &msg)
{
    _socket_manager->SendData(msg);
}

QByteArray LocalSocketClient::ReceiveMessage()
{
    return _socket_manager->ReceiveData();
}

bool LocalSocketClient::HasMessage() const
{
    return _socket_manager->HasData();
}
