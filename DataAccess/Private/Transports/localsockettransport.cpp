#include "localsockettransport.h"

using namespace GQtUtil::DataAccess::Private;

LocalSocketTransport::LocalSocketTransport(QLocalSocket *sock, QObject *parent)
    :ITransportMechanism(parent)
{
    _sock = sock;
}
