#include "localsockettransport.h"

using namespace GUtil::DataAccess::Private;

LocalSocketTransport::LocalSocketTransport(QLocalSocket *sock, QObject *parent)
    :ITransportMechanism(parent)
{
    _sock = sock;
}
