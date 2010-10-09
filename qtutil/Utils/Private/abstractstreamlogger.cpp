#include "abstractstreamlogger.h"
#include <iostream>
using namespace GQtUtil::Utils;
using namespace std;

AbstractStreamLogger::AbstractStreamLogger(ostream *os, PubSubSystem *pss, QObject *parent) :
    AbstractLogger(pss, parent)
{
    _init(os);
}

void AbstractStreamLogger::_init(ostream *os)
{
    _stream = os;
}

void AbstractStreamLogger::LogMessage_protected(const QString &msg, MessageTypeEnum)
{
    LogMessageToStream(msg, _stream);
}

void AbstractStreamLogger::LogMessageToStream(const QString &msg, ostream *stream)
{
    (*stream) << msg.toStdString();
}

std::ostream *AbstractStreamLogger::Stream() const
{
    return _stream;
}
