/*Copyright 2010-2013 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_NO_NETWORK_FUNCTIONALITY

#ifndef GUTIL_LOCAL_SOCKET_IODEVICE_H
#define GUTIL_LOCAL_SOCKET_IODEVICE_H

#include "qt_iodevice.h"
#include <QLocalSocket>

namespace GUtil{ namespace QT{


class LocalSocketIODevice :
        public QT_IODevice
{
    Q_OBJECT
public:

    explicit LocalSocketIODevice(QLocalSocket *,
                                  QObject *parent = 0);

    QLocalSocket &Socket(){
        return (QLocalSocket &)GetIODevice();
    }
    const QLocalSocket &Socket() const{
        return (const QLocalSocket &)GetIODevice();
    }

    bool IsConnected() const{
        return Socket().state() == QLocalSocket::ConnectedState;
    }


signals:

    void Connected(const QUuid &id = QUuid());
    void Disconnected(const QUuid &id = QUuid());
    void Error(const QUuid &id, QLocalSocket::LocalSocketError);
    void StateChanged(const QUuid &id, QLocalSocket::LocalSocketState);


private slots:

    void _localsocket_connected();
    void _localsocket_disconnected();
    void _localsocket_error(QLocalSocket::LocalSocketError);
    void _localsocket_state_changed(QLocalSocket::LocalSocketState);

};


}}

#endif // GUTIL_LOCAL_SOCKET_IODEVICE_H

#endif // NETWORK_FUNCTIONALITY
