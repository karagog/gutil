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

#include "connectionmanager.h"
GUTIL_USING_NAMESPACE( BusinessObjects );

ConnectionManager::ConnectionManager(QObject *parent)
    :QObject(parent)
{}

void ConnectionManager::SendMessage(const QByteArray &message, const QUuid &id)
{
    QList<QUuid> ids;
    if(id.isNull())
        ids = iodevice_manager.GetIds();
    else
        ids.append(id);

    QByteArray newmsg(message);
    newmsg.prepend(":");

    foreach(QUuid i, ids)
        iodevice_manager.SendData(newmsg, i);
}

void ConnectionManager::Reply(const QUuid &msg_id, const QByteArray &message, const QUuid &conn_id)
{
    QList<QUuid> ids;
    if(conn_id.isNull())
        ids = iodevice_manager.GetIds();
    else
        ids.append(conn_id);

    QByteArray newmsg(message);
    newmsg.prepend(QString("%1:").arg(msg_id.toString()).toAscii());

    foreach(QUuid i, ids)
        iodevice_manager.SendData(newmsg, i);
}

QByteArray ConnectionManager::ReceiveMessage(const QUuid &id)
{
    QByteArray ret(iodevice_manager.ReceiveData(id));
    return ret.right(ret.length() - (ret.indexOf(":") + 1));
}


#endif // NETWORK_FUNCTIONALITY
