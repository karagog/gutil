/*Copyright 2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "gutil_uuid.h"
#include "gutil_random.h"
#include <cstring>

#ifndef GUTIL_NO_CRYPTOPP
#include "ThirdParty/cryptopp-5.6.1/osrng.h"
#endif

USING_NAMESPACE_GUTIL1(Utils);


NAMESPACE_GUTIL1(DataObjects);


#ifdef UUID_VERSION_OFFSET
#undef UUID_VERSION_OFFSET
#endif
#ifdef UUID_RESERVED_OFFSET
#undef UUID_RESERVED_OFFSET
#endif


/** The byte offset of the uuid version information */
#define UUID_VERSION_OFFSET     6

/** The byte offset of the uuid reserved bits */
#define UUID_RESERVED_OFFSET    8


Uuid::Uuid(const String &s)
{
    bool init_failed( true );

    if(!s.IsEmpty())
    {
        init_failed = false;

        try
        {
            String cpy1( s.Replace("-", "") );

            // Microsoft's GUIDS start and end with {}, so remove them if they exist
            if(cpy1[cpy1.Length() - 1] == '}')
                cpy1.Chop(1);
            if(cpy1[0] == '{')
                cpy1.RemoveBytesAt(0U);

            String cpy2( cpy1.FromBase16() );

            GASSERT(sizeof(m_data) == cpy2.Length());

            memcpy(m_data, cpy2.ConstData(), sizeof(m_data));
        }
        catch(...)
        {
            init_failed = true;
        }
    }

    // If initialization fails for any reason, then we set ourselves to Nil
    if(init_failed)
        Clear();
}

Uuid::Uuid(const Uuid &other)
{
    memcpy(m_data, other.m_data, sizeof(m_data));
}

void Uuid::Clear()
{
    memset(m_data, 0, sizeof(m_data));
}

void Uuid::Generate()
{
    // Fill with random data
    RandomData::Fill(m_data, sizeof(m_data));

    // Then set the reserved and version bits to conform with the UUID standard

    //   First zero out the bits we want to set
    m_data[UUID_VERSION_OFFSET]  &= (GBYTE)0x0F;
    m_data[UUID_RESERVED_OFFSET] &= (GBYTE)0x3F;

    //   Then set the bits

    //     Since we generate the UUID with random (or pseudo-random) data, this is version 4,
    //       according to the standard
    m_data[UUID_VERSION_OFFSET]  |= (GBYTE)0x40;

    //     The two reserved bits are set to 1 and 0
    m_data[UUID_RESERVED_OFFSET] |= (GBYTE)0x80;
}

String Uuid::ToString() const
{
    String s( (const char *)m_data, sizeof(m_data) );
    s = s.ToBase16();
    s.Insert("-", 1, 8);
    s.Insert("-", 1, 13);
    s.Insert("-", 1, 18);
    s.Insert("-", 1, 23);
    return s;
}

bool Uuid::IsNull() const
{
    bool ret(true);
    GBYTE const *cur(m_data), *end(m_data + sizeof(m_data));
    while(ret && cur != end)
        if(*(cur++)) ret = false;
    return ret;
}

bool Uuid::operator == (const Uuid &other) const
{
    return 0 == memcmp(m_data, other.m_data, sizeof(m_data));
}

bool Uuid::operator != (const Uuid &other) const
{
    return 0 != memcmp(m_data, other.m_data, sizeof(m_data));
}


END_NAMESPACE_GUTIL1;
