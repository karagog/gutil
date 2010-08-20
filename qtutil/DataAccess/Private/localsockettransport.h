#ifndef LOCALSOCKETTRANSPORT_H
#define LOCALSOCKETTRANSPORT_H

#include "io_device_transport.h"

class QLocalSocket;

namespace GQtUtil
{
    namespace DataAccess
    {
        namespace Private
        {
            class LocalSocketTransport : public IODevice_Transport
            {
                Q_OBJECT
            public:
                explicit LocalSocketTransport(QLocalSocket *sock, QObject *parent = 0);

            };
        }
    }
}

#endif // LOCALSOCKETTRANSPORT_H
