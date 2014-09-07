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

#include "gutil_iointerface.h"
#include "gutil_iprogresshandler.h"

namespace CryptoPP{
class HashTransformation;
}

NAMESPACE_GUTIL1(CryptoPP);


/** All the different hash types that are supported by the GUtil::CryptoPP library. */
enum HashAlgorithmEnum
{
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

    You can use this class in several ways:

    - If you have all the data in memory, you can call ComputeHash()
    - If you are only getting data piecemeal, you can call AddData() and Final()
    - If you have a generic IInput, you can use the other ComputeHash() call
        which takes care of buffering the input and also allows you to monitor progress.
*/
class HashBase
{
    GUTIL_DISABLE_COPY(HashBase);
    ::CryptoPP::HashTransformation *m_hash;
public:

    /** Instantiates a new Hash object of the given type. The caller gets ownership.
        This is useful if you don't know the type of hash at compile time, otherwise
        you should instantiate the Hash template object directly.
    */
    static HashBase *CreateHash(HashAlgorithmEnum);

    /** Returns the size of the digest for this hash object. */
    int DigestSize() const;

    /** Adds data to be included in the hash.  When you've added all the data you want,
     *  call Final() to get the digest.
    */
    void AddData(byte const *, GUINT32 len);

    /** Adds data to be included in the hash.  When you've added all the data you want,
     *  call Final() to get the digest.
     *
     *  \param chunk_size The number of bytes to read at a time.
     *  \param ph An optional progress handler to monitor this potentially long operation.
    */
    void AddData(GUtil::IInput *,
                 GUINT32 chunk_size = 0,
                 GUtil::IProgressHandler *ph = 0);

    /** Outputs the digest to digest_out and restarts for a new hash.
     *  \param digest_out Must be the correct digest size, returned by DigestSize()
    */
    void Final(byte *digest_out);

    /** Generates the hash of a fixed length data input. You can initialize the hash
     *  with salt simply by calling AddData() before this.
     *  \param digest_out Must be the correct digest size, returned by DigestSize()
    */
    void ComputeHash(byte *digest_out, byte const *data, GUINT32 data_len){
        AddData(data, data_len);
        Final(digest_out);
    }

    /** Generates the hash of data from an input device. The input device may be null,
     *  in which case you get the hash of zero bytes. You can initialize the hash
     *  with salt simply by calling AddData() before this.
     *
     *  \param digest_out Must be the correct digest size, returned by DigestSize()
     *  \param chunk_size The number of bytes to read at a time.
     *  \param ph An optional progress handler to monitor this potentially long operation.
    */
    void ComputeHash(byte *digest_out,
                     GUtil::IInput *input,
                     GUINT32 chunk_size = 0,
                     GUtil::IProgressHandler *ph = 0){
        AddData(input, chunk_size, ph);
        Final(digest_out);
    }

    /** The destructor is virtual so you can safely delete the base object, and it also includes
        runtime type information for the Hash classes, so you can dynamic_cast between them if you like.
    */
    virtual ~HashBase();


protected:

    HashBase(::CryptoPP::HashTransformation *);
    ::CryptoPP::HashTransformation *get_hash_transformation() const{ return m_hash; }

};


/** Generic template base type does nothing. */
template<HashAlgorithmEnum>class Hash {};


// Here are all the template specializations for our hash classes
#define DECLARE_HASH( HASH_TYPE ) template<>class Hash<HASH_TYPE> : public HashBase { public: Hash(); ~Hash(); }

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
