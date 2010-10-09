#include "streamlogger.h"
#include <iostream>
using namespace GQtUtil::Utils;
using namespace std;

StreamLogger::StreamLogger(PubSubSystem *pss, QObject *parent)
    :AbstractLogger(pss, parent)
{
    _init(&cout);
}

StreamLogger::StreamLogger(ostream *os, PubSubSystem *pss, QObject *parent) :
    AbstractLogger(pss, parent)
{
    _init(os);
}

void StreamLogger::_init(ostream *os)
{
    _stream = os;
}

void StreamLogger::LogMessage_protected(const QString &msg, MessageTypeEnum)
{
    LogMessageToStream(msg, _stream);
}

void StreamLogger::LogMessageToStream(const QString &msg, ostream *stream)
{
    (*stream) << msg.toStdString();
}

std::ostream *StreamLogger::Stream() const
{
    return _stream;
}
