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

#include "tcpsocketclient.h"
#include "DataAccess/gtcpsocketiodevice.h"
#include <QTcpSocket>
GUTIL_USING_NAMESPACE(BusinessObjects);
GUTIL_USING_NAMESPACE(DataAccess);

TcpSocketClient::TcpSocketClient(QObject *parent)
    :ClientBase(parent)
{}

void TcpSocketClient::ConnectToServer(const QHostAddress &address, quint16 port)
{
    GTcpSocketIODevice *sock(new GTcpSocketIODevice(new QTcpSocket(this), this));

    sock->Socket().connectToHost(address, port);
    if(sock->Socket().waitForConnected(5000))
    {
        connect(&sock->Socket(), SIGNAL(disconnected()),
                this, SLOT(disconnected_from_server()));
        iodevice_manager.AddToBundle(sock);
    }
    else
    {
        Core::Exception ex(QString("Unable to connect to host address %1 and port %2")
                           .arg(address.toString())
                           .arg(port)
                           .toStdString());
        ex.SetData("error", sock->Socket().errorString().toStdString());
        delete sock;
        THROW_GUTIL_EXCEPTION(ex);
    }
}

void TcpSocketClient::DisconnectFromServer()
{
    ((GTcpSocketIODevice &)iodevice_manager.Transport()).Socket().disconnectFromHost();
}

QHostAddress TcpSocketClient::Address() const
{
    return ((GTcpSocketIODevice &)iodevice_manager.Transport()).Socket().peerAddress();
}
quint16 TcpSocketClient::Port() const
{
    return ((GTcpSocketIODevice &)iodevice_manager.Transport()).Socket().peerPort();
}

#endif // NETWORK_FUNCTIONALITY
