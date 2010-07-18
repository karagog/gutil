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

#include "sockettransport.h"
#include <QLocalSocket>
using namespace GQtUtil::DataAccess::Private::Transports;

SocketTransport::SocketTransport()
{
    _sock = 0;
}

SocketTransport::SocketTransport(QLocalSocket *socket)
{
    _sock = socket;
}

SocketTransport::~SocketTransport()
{
    if(_sock)
        _sock->close();
}

QLocalSocket *SocketTransport::socket() const
{
    return _sock;
}

void SocketTransport::setSocket(QLocalSocket *socket)
{
    _sock = socket;
}

void SocketTransport::send_data(const QByteArray &data)
{
    if(_sock)
        _sock->write(data);
}

QByteArray SocketTransport::receive_data()
{
    QByteArray ret;
    if(_sock)
        ret = _sock->readAll();
    return ret;
}
