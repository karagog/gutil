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

#include "glocalsocketiodevice.h"

NAMESPACE_GUTIL2(QT, DataAccess);


GLocalSocketIODevice::GLocalSocketIODevice(QLocalSocket *s, QObject *parent)
    :GQIODevice(s, parent)
{
    connect(s, SIGNAL(connected()), this, SLOT(_localsocket_disconnected()));
    connect(s, SIGNAL(disconnected()), this, SLOT(_localsocket_disconnected()));
    connect(s, SIGNAL(error(QLocalSocket::LocalSocketError)),
            this, SLOT(_localsocket_error(QLocalSocket::LocalSocketError)));
    connect(s, SIGNAL(stateChanged(QLocalSocket::LocalSocketState)),
            this, SLOT(_localsocket_state_changed(QLocalSocket::LocalSocketState)));
}

void GLocalSocketIODevice::_localsocket_disconnected()
{
    emit Disconnected(GetIdentity());
}

void GLocalSocketIODevice::_localsocket_error(QLocalSocket::LocalSocketError err)
{
    emit Error(GetIdentity(), err);
}

void GLocalSocketIODevice::_localsocket_connected()
{
    emit Connected(GetIdentity());
}

void GLocalSocketIODevice::_localsocket_state_changed(QLocalSocket::LocalSocketState s)
{
    emit StateChanged(GetIdentity(), s);
}


END_NAMESPACE_GUTIL2;

#endif // NETWORK_FUNCTIONALITY
