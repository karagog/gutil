#include "localsockettransport.h"

using namespace GQtUtil::DataAccess::Private;

LocalSocketTransport::LocalSocketTransport(QLocalSocket *sock, QObject *parent) :
    IODevice_Transport((QIODevice *)sock, parent)
{
}
