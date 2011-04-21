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

#ifndef GTCPSOCKETIODEVICE_H
#define GTCPSOCKETIODEVICE_H

#include "gqiodevice.h"
#include <QTcpSocket>

namespace GUtil{ namespace DataAccess{


class GTcpSocketIODevice :
        public GQIODevice
{
    Q_OBJECT
public:

    explicit GTcpSocketIODevice(QTcpSocket *, QObject *parent = 0);

    inline QTcpSocket &Socket(){
        return (QTcpSocket &)IODevice();
    }
    inline const QTcpSocket &Socket() const{
        return (const QTcpSocket &)IODevice();
    }

    inline bool IsConnected() const{
        return Socket().state() == QTcpSocket::ConnectedState;
    }


signals:

    void Connected(const QUuid &id = QUuid());
    void Disconnected(const QUuid &id = QUuid());
    void Error(const QUuid &id, QTcpSocket::SocketError);
    void StateChanged(const QUuid &id, QTcpSocket::SocketState);


private slots:

    void _tcpsocket_connected();
    void _tcpsocket_disconnected();
    void _tcpsocket_error(QTcpSocket::SocketError);
    void _tcpsocket_state_changed(QTcpSocket::SocketState);

};


}}

#endif // GTCPSOCKETIODEVICE_H

#endif // NETWORK_FUNCTIONALITY
