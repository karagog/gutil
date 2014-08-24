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


/** Hash functions built on Crypto++. */
class Hash
{
public:

    /** All the different hash types that are supported. */
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
        SHA224,
        SHA256,
        SHA384,
        SHA512
    };

    /** Creates a hash object which will use the given algorithm. */
    Hash(HashAlgorithmEnum);
    ~Hash();

    /** Adds data to be included in the hash.  When you've added all the data you want,
     *  call Final() to get the digest.
    */
    void AddData(byte const *, GUINT32 len);

    /** Outputs the digest to digest_out and restarts for a new hash.
     *  \param digest_out Must be the correct digest size, returned by DigestSize()
    */
    void Final(byte *digest_out);

    /** Returns the size of the digest for this hash object */
    int DigestSize() const{ return DigestSize(m_hashType); }

    /** Returns the size of the digest for any type of supported hash algorithm. */
    static int DigestSize(HashAlgorithmEnum);

    /** Generates the hash of a fixed length data input.
     *  \param digest_out Must be the correct digest size, returned by DigestSize()
    */
    static void ComputeHash(byte *digest_out, byte const *data, GUINT32 data_len, HashAlgorithmEnum);

    /** Generates the hash of data from an input device.
     *  \param digest_out Must be the correct digest size, returned by DigestSize()
     *  \param chunk_size The number of bytes to read at a time.
     *  \param ph An optional progress handler to monitor this potentially long operation.
    */
    static void ComputeHash(byte *digest_out,
                            GUtil::InputInterface *,
                            HashAlgorithmEnum,
                            GUINT32 chunk_size = 0, GUtil::IProgressHandler *ph = 0);


private:

    const HashAlgorithmEnum m_hashType;
    ::CryptoPP::HashTransformation *m_hash;

};


END_NAMESPACE_GUTIL1;

#endif // CRYPTOPP_HASH_H
