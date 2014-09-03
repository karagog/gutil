/*Copyright 2014 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "sourcesandsinks.h"
#include "gutil_globals.h"
#include <cstring>

NAMESPACE_GUTIL;


ByteArrayInput::~ByteArrayInput(){}

GUINT32 ByteArrayInput::ReadBytes(byte *buf, GUINT32 len, GUINT32 to_read)
{
    GUINT32 ret = Min(len, to_read);
    ret = Min((GUINT64)ret, BytesAvailable());
    memcpy(buf, m_cur, ret);
    m_cur += ret;
    return ret;
}


END_NAMESPACE_GUTIL;
