/*Copyright 2010-2012 George Karagoulis

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

#ifndef GLocalSocketIODevice_H
#define GLocalSocketIODevice_H

#include "gqiodevice.h"
#include <QLocalSocket>

namespace GUtil{ namespace QT{ namespace DataAccess{


class GLocalSocketIODevice :
        public GQIODevice
{
    Q_OBJECT
public:

    explicit GLocalSocketIODevice(QLocalSocket *,
                                  QObject *parent = 0);

    inline QLocalSocket &Socket(){
        return (QLocalSocket &)IODevice();
    }
    inline const QLocalSocket &Socket() const{
        return (const QLocalSocket &)IODevice();
    }

    inline bool IsConnected() const{
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


}}}

#endif // GLocalSocketIODevice_H

#endif // NETWORK_FUNCTIONALITY
