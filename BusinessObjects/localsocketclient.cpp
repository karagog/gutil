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

#include "localsocketclient.h"
#include "DataAccess/gsocketiodevice.h"
using namespace GUtil;
using namespace DataAccess;
using namespace BusinessObjects;

LocalSocketClient::LocalSocketClient(QObject *parent)
    :QObject(parent)
{
    connect(&_socket_manager, SIGNAL(NewDataArrived(QUuid, QByteArray)),
            this, SLOT(_socket_new_data(QUuid, QByteArray)));
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
                this, SLOT(_disconnected_from_server()));
        _socket_manager.AddToBundle(sock);
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

void LocalSocketClient::DisconnectFromServer()
{
    ((GSocketIODevice &)_socket_manager.Transport())
            .Socket().disconnectFromServer();
}

void LocalSocketClient::_disconnected_from_server()
{
    emit Disconnected();
    _socket_manager.RemoveAll();
}

void LocalSocketClient::_socket_new_data(const QUuid &, const QByteArray &data)
{
    emit NewMessageArrived(QUuid(data.left(data.indexOf(":")).constData()));
}

void LocalSocketClient::SendMessage(const QByteArray &msg)
{
    _socket_manager.SendData(QByteArray(msg).prepend(":"));
}

void LocalSocketClient::Reply(const QUuid &message_id, const QByteArray &data)
{
    _socket_manager.SendData(QByteArray(data).prepend(
                                 QString("%1:")
                                 .arg(message_id.toString()).toAscii()));
}

QByteArray LocalSocketClient::ReceiveMessage()
{
    QByteArray ret(_socket_manager.ReceiveData());
    return ret.right(ret.length() - (ret.indexOf(":") + 1));
}


#endif // NETWORK_FUNCTIONALITY
