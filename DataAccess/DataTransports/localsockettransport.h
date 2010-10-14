#ifndef LOCALSOCKETTRANSPORT_H
#define LOCALSOCKETTRANSPORT_H

#include "abstractdatatransportmechanism.h"

class QLocalSocket;

namespace GUtil
{
    namespace DataAccess
    {
        namespace DataTransports
        {
            class LocalSocketTransport : public AbstractDataTransportMechanism
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
