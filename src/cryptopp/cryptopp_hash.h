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

#ifndef CRYPTOPP_HASH_H
#define CRYPTOPP_HASH_H

#include "gutil_ihash.h"

namespace CryptoPP{
class HashTransformation;
}

NAMESPACE_GUTIL1(CryptoPP);


/** All the different hash types that are supported by the GUtil::CryptoPP library. */
enum HashAlgorithmEnum
{
    NoHash,
    MD2,
    MD4,
    MD5,
    RIPEMD128,
    RIPEMD160,
    RIPEMD256,
    RIPEMD320,
    SHA1,
    Tiger,
    Whirlpool,
    SHA2_224,
    SHA2_256,
    SHA2_384,
    SHA2_512,
    SHA3_224,
    SHA3_256,
    SHA3_384,
    SHA3_512
};


/** The base class for all hash classes built on top of Crypto++. You cannot instantiate it directly, but
    you can create a Hash<> object which derives from this. Alternately you can use the CreateHash()
    static function to instantiate the proper hash object.

    This class is re-entrant, as long as the Crypto++ classes remain re-entrant.
*/
template<HashAlgorithmEnum ALGORITHM = NoHash>
class Hash : public GUtil::IHash
{
    Hash(const Hash<> &) = delete;
    Hash<> &operator = (const Hash<> &) = delete;
public:

    /** Instantiates a new Hash object of the given type. The caller gets ownership.
        This is useful if you don't know the type of hash at compile time, otherwise
        you should instantiate the Hash template object directly.
    */
    static Hash<> *CreateHash(HashAlgorithmEnum);
    virtual ~Hash();
    
    /** Derived classes must return their hash algorithm type. */
    virtual HashAlgorithmEnum GetHashAlgorithm() const = 0;

    /** \name GUtil::IHash interface. 
        \note There is no Clone() implementation because this is an incomplete type.
        \{
    */
    virtual GUINT32 DigestSize() const;
    virtual void AddData(byte const *, GUINT32 len);
    virtual void Final(byte *digest_out);
    /** \} */


protected:

    /** This is an incomplete type, so only derived classes are allowed to construct us. */
    Hash(::CryptoPP::HashTransformation *);
    
    /** Derived classes get direct access to the hash transformation, but they can't
        change it.
    */
    ::CryptoPP::HashTransformation *const hash;

};


/** Generic template base type does nothing. */
template<HashAlgorithmEnum>class Hash;


// Here are all the template specializations for our hash classes
#define DECLARE_HASH( HASH_TYPE ) template<>class Hash<HASH_TYPE> : public Hash<> { \
    GUTIL_DISABLE_COPY(Hash<HASH_TYPE>); \
public: \
    Hash(); \
    virtual ~Hash(); \
    virtual HashAlgorithmEnum GetHashAlgorithm() const; \
    virtual GUtil::IClonable *Clone() const noexcept; \
}

DECLARE_HASH(SHA3_512);
DECLARE_HASH(SHA3_384);
DECLARE_HASH(SHA3_256);
DECLARE_HASH(SHA3_224);

DECLARE_HASH(SHA2_512);
DECLARE_HASH(SHA2_384);
DECLARE_HASH(SHA2_256);
DECLARE_HASH(SHA2_224);

DECLARE_HASH(RIPEMD320);
DECLARE_HASH(RIPEMD256);
DECLARE_HASH(RIPEMD160);
DECLARE_HASH(RIPEMD128);

DECLARE_HASH(Whirlpool);
DECLARE_HASH(Tiger);

DECLARE_HASH(SHA1);
DECLARE_HASH(MD5);
DECLARE_HASH(MD4);
DECLARE_HASH(MD2);


END_NAMESPACE_GUTIL1;

#endif // CRYPTOPP_HASH_H
