#include "localsockettransport.h"
using namespace GUtil;

DataAccess::LocalSocketTransport::LocalSocketTransport(QLocalSocket *sock, QObject *parent)
    :AbstractDataTransportMechanism(parent)
{
    _sock = sock;
}
