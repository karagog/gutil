#ifndef LOCALSOCKETTRANSPORT_H
#define LOCALSOCKETTRANSPORT_H

#include "abstractqiodevicetransport.h"
#include <QLocalSocket>

namespace GUtil
{
    namespace DataAccess
    {
        class LocalSocketTransport : public AbstractQIODeviceTransport
        {
            Q_OBJECT
        public:
            explicit LocalSocketTransport(QLocalSocket *sock = 0, QObject *parent = 0);

            void ConnectToServer(const QString &) throw(GUtil::Core::DataTransportException);
            void DisconnectFromServer();

            QString ServerName() const;

        protected:
            QLocalSocket &Socket() const;

        };
    }
}

#endif // LOCALSOCKETTRANSPORT_H
