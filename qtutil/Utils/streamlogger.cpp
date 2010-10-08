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
    // Open it
    if(PrepareMessageStream())
    {
        // Write to the stream
        LogMessageToStream(msg);

        // Close it
        CloseMessageStream();
    }
}

bool StreamLogger::PrepareMessageStream()
{
    // Do nothing by default
    return true;
}

void StreamLogger::LogMessageToStream(const QString &msg)
{
    LogMessageToStream(msg, _stream);
}

void StreamLogger::LogMessageToStream(const QString &msg, ostream *stream)
{
    (*stream) << msg.toStdString();
}

void StreamLogger::CloseMessageStream()
{
    // Do nothing by default
}

std::ostream *StreamLogger::Stream() const
{
    return _stream;
}
