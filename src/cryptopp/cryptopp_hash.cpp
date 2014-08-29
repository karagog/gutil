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
#include <cryptopp/sha3.h>
#include <cryptopp/tiger.h>
#include <cryptopp/whrlpool.h>
USING_NAMESPACE_GUTIL;

NAMESPACE_GUTIL1(CryptoPP);


HashBase::HashBase(::CryptoPP::HashTransformation *h)
    :m_hash(h) {}

HashBase::~HashBase() {}

int HashBase::DigestSize() const
{
    return get_hash_transformation()->DigestSize();
}

void HashBase::AddData(const byte *d, GUINT32 len)
{
    get_hash_transformation()->Update(d, len);
}

void HashBase::Final(byte *d)
{
    get_hash_transformation()->Final(d);
}

void HashBase::ComputeHash(byte *out,
                       InputInterface *input,
                       GUINT32 chunk_size,
                       IProgressHandler *ph)
{
    GUINT32 len = input ? input->BytesAvailable() : 0;
    GUINT32 to_read = chunk_size == 0 ? len : GUtil::Min(chunk_size, len);
    GUINT32 read = 0;
    const GUINT32 buf_sz = to_read;

    SmartArrayPointer<byte> buf( new byte[buf_sz] );
    if(ph) ph->ProgressUpdated(0);
    while(read < len)
    {
        if(to_read != input->ReadBytes(buf, buf_sz, to_read))
            THROW_NEW_GUTIL_EXCEPTION2(Exception, "Error reading from device");

        AddData(buf, buf_sz);
        read += to_read;

        if((read + to_read) > len)
            to_read = len - read;

        if(ph){
            if(ph->CancelOperation())
                THROW_NEW_GUTIL_EXCEPTION(CancelledOperationException);
            ph->ProgressUpdated(((float)read / len) * 100);
        }
    }
    Final(out);
    if(ph) ph->ProgressUpdated(100);
}

HashBase *HashBase::CreateHash(HashAlgorithmEnum h)
{
    HashBase *ret;
    switch(h)
    {
    case SHA3_512:
        ret = new Hash<SHA3_512>;
        break;
    case SHA3_384:
        ret = new Hash<SHA3_384>;
        break;
    case SHA3_256:
        ret = new Hash<SHA3_256>;
        break;
    case SHA3_224:
        ret = new Hash<SHA3_224>;
        break;
    case SHA512:
        ret = new Hash<SHA512>;
        break;
    case SHA384:
        ret = new Hash<SHA384>;
        break;
    case SHA256:
        ret = new Hash<SHA256>;
        break;
    case SHA224:
        ret = new Hash<SHA224>;
        break;
    case SHA1:
        ret = new Hash<SHA1>;
        break;
    case Tiger:
        ret = new Hash<Tiger>;
        break;
    case Whirlpool:
        ret = new Hash<Whirlpool>;
        break;
    case RIPEMD320:
        ret = new Hash<RIPEMD320>;
        break;
    case RIPEMD256:
        ret = new Hash<RIPEMD256>;
        break;
    case RIPEMD160:
        ret = new Hash<RIPEMD160>;
        break;
    case RIPEMD128:
        ret = new Hash<RIPEMD128>;
        break;
    case MD5:
        ret = new Hash<MD5>;
        break;
    case MD4:
        ret = new Hash<MD4>;
        break;
    case MD2:
        ret = new Hash<MD2>;
        break;
    default:
        THROW_NEW_GUTIL_EXCEPTION(NotImplementedException);
    }
    return ret;
}


Hash<SHA3_512>::Hash() :HashBase(new ::CryptoPP::SHA3_512) {}
Hash<SHA3_512>::~Hash() { delete static_cast< ::CryptoPP::SHA3_512 *>(get_hash_transformation()); }

Hash<SHA3_384>::Hash() :HashBase(new ::CryptoPP::SHA3_384) {}
Hash<SHA3_384>::~Hash() { delete static_cast< ::CryptoPP::SHA3_384 *>(get_hash_transformation()); }

Hash<SHA3_256>::Hash() :HashBase(new ::CryptoPP::SHA3_256) {}
Hash<SHA3_256>::~Hash() { delete static_cast< ::CryptoPP::SHA3_256 *>(get_hash_transformation()); }

Hash<SHA3_224>::Hash() :HashBase(new ::CryptoPP::SHA3_224) {}
Hash<SHA3_224>::~Hash() { delete static_cast< ::CryptoPP::SHA3_224 *>(get_hash_transformation()); }


Hash<SHA512>::Hash() :HashBase(new ::CryptoPP::SHA512) {}
Hash<SHA512>::~Hash() { delete static_cast< ::CryptoPP::SHA512 *>(get_hash_transformation()); }

Hash<SHA384>::Hash() :HashBase(new ::CryptoPP::SHA384) {}
Hash<SHA384>::~Hash() { delete static_cast< ::CryptoPP::SHA384 *>(get_hash_transformation()); }

Hash<SHA256>::Hash() :HashBase(new ::CryptoPP::SHA256) {}
Hash<SHA256>::~Hash() { delete static_cast< ::CryptoPP::SHA256 *>(get_hash_transformation()); }

Hash<SHA224>::Hash() :HashBase(new ::CryptoPP::SHA224) {}
Hash<SHA224>::~Hash() { delete static_cast< ::CryptoPP::SHA224 *>(get_hash_transformation()); }


Hash<RIPEMD320>::Hash() :HashBase(new ::CryptoPP::RIPEMD320) {}
Hash<RIPEMD320>::~Hash() { delete static_cast< ::CryptoPP::RIPEMD320 *>(get_hash_transformation()); }

Hash<RIPEMD256>::Hash() :HashBase(new ::CryptoPP::RIPEMD256) {}
Hash<RIPEMD256>::~Hash() { delete static_cast< ::CryptoPP::RIPEMD256 *>(get_hash_transformation()); }

Hash<RIPEMD160>::Hash() :HashBase(new ::CryptoPP::RIPEMD160) {}
Hash<RIPEMD160>::~Hash() { delete static_cast< ::CryptoPP::RIPEMD160 *>(get_hash_transformation()); }

Hash<RIPEMD128>::Hash() :HashBase(new ::CryptoPP::RIPEMD128) {}
Hash<RIPEMD128>::~Hash() { delete static_cast< ::CryptoPP::RIPEMD128 *>(get_hash_transformation()); }


Hash<Tiger>::Hash() :HashBase(new ::CryptoPP::Tiger) {}
Hash<Tiger>::~Hash() { delete static_cast< ::CryptoPP::Tiger *>(get_hash_transformation()); }

Hash<Whirlpool>::Hash() :HashBase(new ::CryptoPP::Whirlpool) {}
Hash<Whirlpool>::~Hash() { delete static_cast< ::CryptoPP::Whirlpool *>(get_hash_transformation()); }


Hash<SHA1>::Hash() :HashBase(new ::CryptoPP::SHA1) {}
Hash<SHA1>::~Hash() { delete static_cast< ::CryptoPP::SHA1 *>(get_hash_transformation()); }


Hash<MD5>::Hash() :HashBase(new ::CryptoPP::Weak1::MD5) {}
Hash<MD5>::~Hash() { delete static_cast< ::CryptoPP::Weak1::MD5 *>(get_hash_transformation()); }

Hash<MD4>::Hash() :HashBase(new ::CryptoPP::Weak1::MD4) {}
Hash<MD4>::~Hash() { delete static_cast< ::CryptoPP::Weak1::MD4 *>(get_hash_transformation()); }

Hash<MD2>::Hash() :HashBase(new ::CryptoPP::Weak1::MD2) {}
Hash<MD2>::~Hash() { delete static_cast< ::CryptoPP::Weak1::MD2 *>(get_hash_transformation()); }


END_NAMESPACE_GUTIL1;
