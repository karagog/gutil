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

#include "cryptopp_hash.h"
#include "gutil_smartpointer.h"
#include "gutil_globals.h"

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/md2.h>
#include <cryptopp/md4.h>
#include <cryptopp/md5.h>
#include <cryptopp/ripemd.h>
#include <cryptopp/sha.h>
#include <cryptopp/tiger.h>
#include <cryptopp/whrlpool.h>
USING_NAMESPACE_GUTIL;

NAMESPACE_GUTIL1(CryptoPP);


Hash::Hash(HashAlgorithmEnum ht)
    :m_hashType(ht)
{
    switch(ht)
    {
    case MD2:
        m_hash = new ::CryptoPP::Weak1::MD2;
        break;
    case MD4:
        m_hash = new ::CryptoPP::Weak1::MD4;
        break;
    case MD5:
        m_hash = new ::CryptoPP::Weak1::MD5;
        break;
    case RIPEMD128:
        m_hash = new ::CryptoPP::RIPEMD128;
        break;
    case RIPEMD160:
        m_hash = new ::CryptoPP::RIPEMD160;
        break;
    case RIPEMD256:
        m_hash = new ::CryptoPP::RIPEMD256;
        break;
    case RIPEMD320:
        m_hash = new ::CryptoPP::RIPEMD320;
        break;
    case Tiger:
        m_hash = new ::CryptoPP::Tiger;
        break;
    case SHA1:
        m_hash = new ::CryptoPP::SHA1;
        break;
    case SHA224:
        m_hash = new ::CryptoPP::SHA224;
        break;
    case SHA256:
        m_hash = new ::CryptoPP::SHA256;
        break;
    case SHA384:
        m_hash = new ::CryptoPP::SHA384;
        break;
    case SHA512:
        m_hash = new ::CryptoPP::SHA512;
        break;
    case Whirlpool:
        m_hash = new ::CryptoPP::Whirlpool;
        break;
    default:
        THROW_NEW_GUTIL_EXCEPTION(Exception);
    }
}

Hash::~Hash()
{
    // Because HashTransformation doesn't have a virtual destructor, we have to
    // go through this mess...
    switch(m_hashType)
    {
    case MD2:
        delete static_cast< ::CryptoPP::Weak1::MD2 *>(m_hash);
        break;
    case MD4:
        delete static_cast< ::CryptoPP::Weak1::MD4 *>(m_hash);
        break;
    case MD5:
        delete static_cast< ::CryptoPP::Weak1::MD5 *>(m_hash);
        break;
    case RIPEMD128:
        delete static_cast< ::CryptoPP::RIPEMD128 *>(m_hash);
        break;
    case RIPEMD160:
        delete static_cast< ::CryptoPP::RIPEMD160 *>(m_hash);
        break;
    case RIPEMD256:
        delete static_cast< ::CryptoPP::RIPEMD256 *>(m_hash);
        break;
    case RIPEMD320:
        delete static_cast< ::CryptoPP::RIPEMD320 *>(m_hash);
        break;
    case Tiger:
        delete static_cast< ::CryptoPP::Tiger *>(m_hash);
        break;
    case SHA1:
        delete static_cast< ::CryptoPP::SHA1 *>(m_hash);
        break;
    case SHA224:
        delete static_cast< ::CryptoPP::SHA224 *>(m_hash);
        break;
    case SHA256:
        delete static_cast< ::CryptoPP::SHA256 *>(m_hash);
        break;
    case SHA384:
        delete static_cast< ::CryptoPP::SHA384 *>(m_hash);
        break;
    case SHA512:
        delete static_cast< ::CryptoPP::SHA512 *>(m_hash);
        break;
    case Whirlpool:
        delete static_cast< ::CryptoPP::Whirlpool *>(m_hash);
        break;
    default:
        THROW_NEW_GUTIL_EXCEPTION(Exception);
    }
}

int Hash::DigestSize(HashAlgorithmEnum h)
{
    int ret = 0;
    switch(h)
    {
    case MD2:
    case MD4:
    case MD5:
    case RIPEMD128:
        ret = ::CryptoPP::Weak1::MD5::DIGESTSIZE;
        break;
    case RIPEMD160:
    case SHA1:
        ret = ::CryptoPP::SHA1::DIGESTSIZE;
        break;
    case Tiger:
        ret = ::CryptoPP::Tiger::DIGESTSIZE;
        break;
    case SHA224:
        ret = ::CryptoPP::SHA224::DIGESTSIZE;
        break;
    case RIPEMD256:
    case SHA256:
        ret = ::CryptoPP::SHA256::DIGESTSIZE;
        break;
    case RIPEMD320:
        ret = ::CryptoPP::RIPEMD320::DIGESTSIZE;
        break;
    case SHA384:
        ret = ::CryptoPP::SHA384::DIGESTSIZE;
        break;
    case Whirlpool:
    case SHA512:
        ret = ::CryptoPP::SHA512::DIGESTSIZE;
        break;
    }
    return ret;
}

void Hash::AddData(const byte *d, GUINT32 len)
{
    m_hash->Update(d, len);
}

void Hash::Final(byte *d)
{
    m_hash->Final(d);
}


void Hash::ComputeHash(byte *out, byte const *data, GUINT32 data_len, HashAlgorithmEnum e)
{
    Hash h(e);
    h.AddData(data, data_len);
    h.Final(out);
}

void Hash::ComputeHash(byte *out,
                       InputInterface *input,
                       HashAlgorithmEnum e,
                       GUINT32 chunk_size,
                       IProgressHandler *ph)
{
    Hash h(e);
    GUINT32 len = input->BytesAvailable();
    GUINT32 to_read = chunk_size == 0 ? len : GUtil::Min(chunk_size, len);
    GUINT32 read = 0;
    const GUINT32 buf_sz = to_read;

    SmartArrayPointer<byte> buf( new byte[buf_sz] );
    if(ph) ph->ProgressUpdated(0);
    while(read < len)
    {
        if(to_read != input->ReadBytes(buf, buf_sz, to_read))
            THROW_NEW_GUTIL_EXCEPTION2(Exception, "Error reading from device");

        h.AddData(buf, buf_sz);
        read += to_read;

        if(ph){
            if(ph->CancelOperation())
                THROW_NEW_GUTIL_EXCEPTION(CancelledOperationException);
            ph->ProgressUpdated(((float)read / len) * 100);
        }
    }
    h.Final(out);
}


END_NAMESPACE_GUTIL1;
