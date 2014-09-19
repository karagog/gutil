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


template<>Hash<>::Hash(::CryptoPP::HashTransformation *h)
    :hash(h) {}

template<>Hash<>::~Hash(){
    // We can't delete the hash here because its destructor is not virtual...
    //  Damn you crypto++!
}

template<>
GUINT32 Hash<>::DigestSize() const
{
    return hash->DigestSize();
}

template<>
void Hash<>::AddData(const byte *d, GUINT32 len)
{
    hash->Update(d, len);
}

template<>
void Hash<>::Final(byte *d)
{
    hash->Final(d);
}

template<>
Hash<> *Hash<>::CreateHash(HashAlgorithmEnum h)
{
    Hash<> *ret;
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
    case SHA2_512:
        ret = new Hash<SHA2_512>;
        break;
    case SHA2_384:
        ret = new Hash<SHA2_384>;
        break;
    case SHA2_256:
        ret = new Hash<SHA2_256>;
        break;
    case SHA2_224:
        ret = new Hash<SHA2_224>;
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
        throw NotImplementedException<>();
    }
    return ret;
}


#define DECLARE_HASH_IMPLEMENTATION(H_MINE, H_CRYPTOPP) \
        Hash<H_MINE>::Hash() :Hash<>(new ::CryptoPP::H_CRYPTOPP) {} \
        Hash<H_MINE>::~Hash() { delete static_cast< ::CryptoPP::H_CRYPTOPP *>(this->hash); } \
        HashAlgorithmEnum Hash<H_MINE>::GetHashAlgorithm() const{ return H_MINE; } \
        IClonable *Hash<H_MINE>::Clone() const noexcept{ return new Hash<H_MINE>; }
        
DECLARE_HASH_IMPLEMENTATION(SHA3_512, SHA3_512);
DECLARE_HASH_IMPLEMENTATION(SHA3_384, SHA3_384);
DECLARE_HASH_IMPLEMENTATION(SHA3_256, SHA3_256);
DECLARE_HASH_IMPLEMENTATION(SHA3_224, SHA3_224);

DECLARE_HASH_IMPLEMENTATION(SHA2_512, SHA512);
DECLARE_HASH_IMPLEMENTATION(SHA2_384, SHA384);
DECLARE_HASH_IMPLEMENTATION(SHA2_256, SHA256);
DECLARE_HASH_IMPLEMENTATION(SHA2_224, SHA224);

DECLARE_HASH_IMPLEMENTATION(RIPEMD320, RIPEMD320);
DECLARE_HASH_IMPLEMENTATION(RIPEMD256, RIPEMD256);
DECLARE_HASH_IMPLEMENTATION(RIPEMD160, RIPEMD160);
DECLARE_HASH_IMPLEMENTATION(RIPEMD128, RIPEMD128);

DECLARE_HASH_IMPLEMENTATION(Tiger, Tiger);
DECLARE_HASH_IMPLEMENTATION(Whirlpool, Whirlpool);

DECLARE_HASH_IMPLEMENTATION(SHA1, SHA1);

DECLARE_HASH_IMPLEMENTATION(MD5, Weak1::MD5);
DECLARE_HASH_IMPLEMENTATION(MD4, Weak1::MD4);
DECLARE_HASH_IMPLEMENTATION(MD2, Weak1::MD2);


END_NAMESPACE_GUTIL1;
