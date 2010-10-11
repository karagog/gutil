#ifndef LOCALSOCKETTRANSPORT_H
#define LOCALSOCKETTRANSPORT_H

#include "Interfaces/ITransportMechanism.h"

class QLocalSocket;

namespace GQtUtil
{
    namespace DataAccess
    {
        namespace Private
        {
            class LocalSocketTransport : public GQtUtil::Interfaces::ITransportMechanism
            {
                Q_OBJECT
            public:
                explicit LocalSocketTransport(QLocalSocket *sock, QObject *parent = 0);


            private:
                QLocalSocket *_sock;

            };
        }
    }
}

#endif // LOCALSOCKETTRANSPORT_H
