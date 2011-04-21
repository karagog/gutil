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

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include "giodevicebundlemanager.h"

namespace GUtil{ namespace BusinessObjects{


// Used as a base class for both the client and server classes
class ConnectionManager :
        public QObject
{
    Q_OBJECT
public:

    explicit ConnectionManager(QObject *parent = 0);
    virtual ~ConnectionManager(){}

    // You can send a generic message, or reply specifically to a
    //  message you received
    void SendMessage(const QByteArray &, const QUuid &conn_id = QUuid());
    void Reply(const QUuid &message_id, const QByteArray &, const QUuid &conn_id = QUuid());

    QByteArray ReceiveMessage(const QUuid &conn_id = QUuid());
    inline bool HasMessage(const QUuid &conn_id = QUuid()){
        return iodevice_manager.HasData(conn_id);
    }


signals:

    void NewMessageArrived(const QUuid &message_id,
                           const QUuid &conn_id = QUuid());


protected:

    GIODeviceBundleManager iodevice_manager;

};


}}

#endif // CONNECTIONMANAGER_H

#endif // NETWORK_FUNCTIONALITY
