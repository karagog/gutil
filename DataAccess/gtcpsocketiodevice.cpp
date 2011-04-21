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

#include "gtcpsocketiodevice.h"
GUTIL_USING_NAMESPACE(DataAccess);

GTcpSocketIODevice::GTcpSocketIODevice(QTcpSocket *tcp, QObject *parent)
    :GQIODevice(tcp, parent)
{
    connect(tcp, SIGNAL(connected()), this, SLOT(_tcpsocket_disconnected()));
    connect(tcp, SIGNAL(disconnected()), this, SLOT(_tcpsocket_disconnected()));
    connect(tcp, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(_tcpsocket_error(QAbstractSocket::SocketError)));
    connect(tcp, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(_tcpsocket_state_changed(QAbstractSocket::SocketState)));
}

void GTcpSocketIODevice::_tcpsocket_connected()
{
    emit Connected(GetIdentity());
}

void GTcpSocketIODevice::_tcpsocket_disconnected()
{
    emit Disconnected(GetIdentity());
}

void GTcpSocketIODevice::_tcpsocket_error(QTcpSocket::SocketError err)
{
    emit Error(GetIdentity(), err);
}

void GTcpSocketIODevice::_tcpsocket_state_changed(QTcpSocket::SocketState s)
{
    emit StateChanged(GetIdentity(), s);
}


#endif // NETWORK_FUNCTIONALITY
