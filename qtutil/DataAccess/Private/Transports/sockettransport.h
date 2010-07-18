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

#ifndef SOCKETTRANSPORT_H
#define SOCKETTRANSPORT_H

#include "ITransportMechanism.h"

class QLocalSocket;

namespace GQtUtil
{
    namespace DataAccess
    {
        namespace Private
        {
            namespace Transports
            {
                class SocketTransport : public ITransportMechanism
                {
                public:
                    SocketTransport();
                    SocketTransport(QLocalSocket *);
                    ~SocketTransport();

                    QLocalSocket *socket();
                    void setSocket(QLocalSocket *);

                protected:
                    virtual void send_data(const QByteArray &);
                    virtual QByteArray receive_data();

                private:
                    QLocalSocket *_sock;
                };
            }
        }
    }
}

#endif // SOCKETTRANSPORT_H
