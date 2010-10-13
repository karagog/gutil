#include "localsockettransport.h"
using namespace GUtil::DataAccess;

DataTransports::LocalSocketTransport::LocalSocketTransport(QLocalSocket *sock, QObject *parent)
    :ITransportMechanism(parent)
{
    _sock = sock;
}
