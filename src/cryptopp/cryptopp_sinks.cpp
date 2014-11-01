/*Copyright 2010-2013 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "cryptopp_sinks.h"
#include <gutil/strings.h>
#include <gutil/iio.h>
#include "cryptopp_hash.h"
#include <QByteArray>

NAMESPACE_GUTIL1(CryptoPP);


GUINT32 HashOutput::WriteBytes(const GBYTE *data, GUINT32 len)
{
    m_hash.AddData(data, len);
    return len;
}

size_t OutputInterfaceSink::Put2(const byte *inString, size_t length, int msg_end, bool blocking)
{
    GUTIL_UNUSED(msg_end);
    GUTIL_UNUSED(blocking);

    ref.WriteBytes(inString, length);
    return length;
}

bool OutputInterfaceSink::IsolatedFlush(bool, bool)
{
    ref.Flush();
    return true;
}


size_t StringSink::Put2(const byte *inString, size_t length, int msg_end, bool blocking)
{
    GUTIL_UNUSED(msg_end);
    GUTIL_UNUSED(blocking);

    ref.Append((const char *)inString, length);
    return length;
}


END_NAMESPACE_GUTIL1;
