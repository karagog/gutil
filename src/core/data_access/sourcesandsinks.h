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

#ifndef GUTIL_SOURCESANDSINKS_H
#define GUTIL_SOURCESANDSINKS_H

#include "gutil_iointerface.h"
#include "gutil_vector.h"

NAMESPACE_GUTIL;


/** Defines a byte array input source.  The byte array must exist
 *  for the lifetime of this instance.  The caller retains ownership
 *  of the byte array.
*/
class ByteArrayInput : public GUtil::IRandomAccessInput
{
    byte const *const m_start;
    byte const *const m_end;
    byte const *m_cur;
public:

    /** Initialize from a byte array. */
    ByteArrayInput(byte const *ba, GUINT32 len)
        :m_start(ba), m_end(m_start + len), m_cur(m_start) {}

    /** Initialize from a char array. */
    ByteArrayInput(char const *ba, GUINT32 len)
        :m_start((byte const *)ba), m_end(m_start + len), m_cur(m_start) {}

    ByteArrayInput(const GUtil::Vector<char> &s)
        :m_start((byte const *)s.ConstData()), m_end(m_start + s.Length()), m_cur(m_start) {}

    ByteArrayInput(const GUtil::Vector<byte> &s)
        :m_start((byte const *)s.ConstData()), m_end(m_start + s.Length()), m_cur(m_start) {}

    virtual ~ByteArrayInput();

    /** \name GUtil::IRandomAccessInput
        \{
    */
    virtual GUINT32 ReadBytes(byte *buf, GUINT32 len, GUINT32 to_read);
    virtual GUINT32 Pos() const{ return m_cur - m_start; }
    virtual GUINT32 Length() const{ return m_end - m_start; }
    virtual void Seek(GUINT32 pos){ m_cur = m_start + pos; }
    /** \} */
};



/** Outputs to a byte array. Use as a data sink. */
class ByteArrayOutput : public GUtil::IOutput
{
    Vector<char> &m_out;
public:

    ByteArrayOutput(Vector<char> &ba) :m_out(ba) {}

    virtual GUINT32 WriteBytes(const byte *data, GUINT32 len){
        m_out.Insert((char const *)data, len, m_out.Length());
        return len;
    }

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_SOURCESANDSINKS_H
