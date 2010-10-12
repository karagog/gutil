/*Copyright 2010 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "abstractstreamlogger.h"
#include <iostream>
using namespace GUtil::Utils;
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

void AbstractStreamLogger::LogMessage_protected(const QString &msg, MessageLevelEnum)
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
