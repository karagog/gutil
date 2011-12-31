/*Copyright 2011 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

/** To disable warnings, because we're exposing some weak algorithms (MD4-5) */
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include "encryptionutils.h"
#include "Core/Utils/smartpointer.h"
#include "cryptopp-5.6.1/cryptlib.h"
#include "cryptopp-5.6.1/filters.h"
#include "cryptopp-5.6.1/gzip.h"
#include "cryptopp-5.6.1/randpool.h"
#include "cryptopp-5.6.1/osrng.h"
#include "cryptopp-5.6.1/default.h"
#include "cryptopp-5.6.1/pwdbased.h"
#include "cryptopp-5.6.1/md4.h"
#include "cryptopp-5.6.1/md5.h"
#include "gstringsink.h"
USING_NAMESPACE_GUTIL1(DataObjects);
USING_NAMESPACE_GUTIL1(Utils);
using namespace CryptoPP;

NAMESPACE_GUTIL1(CryptoPP);


String EncryptionUtils::CompressString(const GBYTE *data, GUINT32 data_len,
                                       EncryptionUtils::CompressionLevelEnum level)
{
    String ret(data_len + 1);   // Preallocate space for the return string
    bool skip_compression = data_len > 10000000;

    if(!skip_compression)
    {
        if(level < MinimumCompression || level > MaximumCompression)
            level = DefaultCompression;

        Gzip zipper(new StringSink(ret), level);
        zipper.Put(data, data_len);
        zipper.MessageEnd();
    }

    if(skip_compression ||  ret.Length() >= data_len)
    {
        // Leave it uncompressed, because we didn't gain anything by compression
        ret = "0";
        ret.Append((const char *)data, data_len);
    }
    else
    {
        ret.Prepend("1");
    }
    return ret;
}

String EncryptionUtils::DecompressString(const GBYTE *data, GUINT32 data_len)
{
    String ret(data_len);

    if(data_len > 0)
    {
        bool is_compressed( false );
        const GBYTE *start(data);

        if(*start == '1')
        {
            is_compressed = true;
            ++start;
            --data_len;
        }
        else if(*start == '0')
        {
            ++start;
            --data_len;
        }
        else
        {
            // Treat any string without our marking as compressed, then you'll get an exception
            //  if we couldn't decompress it
            is_compressed = true;
        }

        if(is_compressed)
        {
            StringSource(start, data_len, true, new Gunzip(new StringSink(ret)));
        }
        else
        {
            ret.Append((const char *)start, data_len);
        }
    }

    return ret;
}


#define ENCRYPTED_MESSAGE_STAMP 0xAAAAAAAAAAAAAAAAULL

String EncryptionUtils::EncryptString(const GBYTE *data, GUINT32 data_len,
                                      const GBYTE *key, GUINT32 len, EncryptionTypeEnum e)
{
    byte padded_key[SHA256::DIGESTSIZE];
    String ret(data_len);   // A heuristic to estimate the length of the return string
    if(len == UINT_MAX)
        len = strlen(reinterpret_cast<const char *>(key));

    SmartPointer<StreamTransformation> mode;

    // At least as big as the largest block size
    byte init_vec[AES::BLOCKSIZE];
    GUINT32 block_size;

    switch(e)
    {
    case DefaultEncryption:
    case AES_Encryption:
    {
        // Initialize the padded key (SHA256 generates 32 bytes, which is the maximum key length for AES)
        SHA256().CalculateDigest(padded_key, reinterpret_cast<const byte *>(key), len);

        block_size = AES::BLOCKSIZE;

        // Create a random initialization vector, which we'll store at the front of the string
        AutoSeededX917RNG<AES>().GenerateBlock(init_vec, block_size);
        mode = new CTR_Mode<AES>::Encryption(padded_key, AES::MAX_KEYLENGTH, init_vec);
    }
        break;
    case DES_Encryption:
    {
        SHA256().CalculateTruncatedDigest(padded_key, DES_EDE2::KEYLENGTH,
                                          reinterpret_cast<const byte *>(key), len);

        block_size = DES_EDE2::BLOCKSIZE;

        // Create a random initialization vector, which we'll store at the front of the string
        AutoSeededX917RNG<AES>().GenerateBlock(init_vec, block_size);

        mode = new CTR_Mode<DES_EDE2>::Encryption(padded_key, DES_EDE2::KEYLENGTH, init_vec);
    }
        break;
    case TripleDES_Encryption:
    {
        // Initialize the padded key
        SHA256().CalculateTruncatedDigest(padded_key, DES_EDE3::KEYLENGTH,
                                          reinterpret_cast<const byte *>(key), len);

        block_size = DES_EDE3::BLOCKSIZE;

        AutoSeededX917RNG<DES_EDE3>().GenerateBlock(init_vec, block_size);
        mode = new CTR_Mode<DES_EDE3>::Encryption(padded_key, DES_EDE3::KEYLENGTH, init_vec);
    }
        break;
    default:
        THROW_NEW_GUTIL_EXCEPTION(NotImplementedException);
        break;
    }

    StreamTransformationFilter enc(*mode, new StringSink(ret));

    // The initialization vector goes on the front of the string, unencrypted
    ret.Append(reinterpret_cast<const char *>(init_vec), block_size);

    // Put the data through the encryption
    enc.Put(reinterpret_cast<const byte *>(data), data_len);

    // We put a stamp on the end of the plaintext message,
    //  so we know later if we have correctly decrypted it
    GUINT64 stamp(ENCRYPTED_MESSAGE_STAMP);
    enc.Put(reinterpret_cast<const byte *>(&stamp), sizeof(GUINT64));

    enc.MessageEnd();

    return ret;
}

String EncryptionUtils::DecryptString(const GBYTE *data, GUINT32 data_len, const GBYTE *key, GUINT32 len, EncryptionTypeEnum e)
{
    String ret(data_len);
    if(len == UINT_MAX)
        len = strlen(reinterpret_cast<const char *>(key));


    // Each encryption algorithm may use a different block size, and therefore require
    //  a different length initialization vector
    GUINT32 iv_len;

    // This just needs to be big enough to hold the maximum padded key length
    byte padded_key[AES::MAX_KEYLENGTH];
    SmartPointer<StreamTransformation> mode;

    switch(e)
    {
    case DefaultEncryption:
    case AES_Encryption:
    {
        iv_len = AES::BLOCKSIZE;

        SHA256().CalculateDigest(padded_key, reinterpret_cast<const byte *>(key), len);
        mode = new CTR_Mode<AES>::Decryption(padded_key, AES::MAX_KEYLENGTH, reinterpret_cast<const byte *>(data));
    }
        break;
    case DES_Encryption:
    {
        iv_len = DES_EDE2::BLOCKSIZE;

        SHA256().CalculateTruncatedDigest(padded_key, DES_EDE2::KEYLENGTH, reinterpret_cast<const byte *>(key), len);
        mode = new CTR_Mode<DES_EDE2>::Decryption(padded_key, DES_EDE2::KEYLENGTH, reinterpret_cast<const byte *>(data));
    }
        break;
    case TripleDES_Encryption:
    {
        iv_len = DES_EDE3::BLOCKSIZE;

        SHA256().CalculateTruncatedDigest(padded_key, DES_EDE3::KEYLENGTH, reinterpret_cast<const byte *>(key), len);
        mode = new CTR_Mode<DES_EDE3>::Decryption(padded_key, DES_EDE3::KEYLENGTH, reinterpret_cast<const byte *>(data));
    }
        break;
    default:
        THROW_NEW_GUTIL_EXCEPTION(NotImplementedException);
        break;
    }

    StreamTransformationFilter decryptor(*mode, new StringSink(ret));
    decryptor.Put(reinterpret_cast<const byte *>(data) + iv_len, data_len - iv_len);
    decryptor.MessageEnd();

    // Check to see if the stamp is there, and remove it
    GUINT64 stamp(ENCRYPTED_MESSAGE_STAMP);
    if(0 == memcmp(&stamp, ret.ConstData() + ret.Length() - sizeof(GUINT64), sizeof(GUINT64)))
        ret.Chop(sizeof(GUINT64));
    else
        THROW_NEW_GUTIL_EXCEPTION(Exception);

    return ret;
}

String EncryptionUtils::Hash(const GBYTE *data, GUINT32 data_len, HashAlgorithmEnum e)
{
    GUINT32 str_len;
    SmartPointer<HashTransformation> h;

    if(data_len == UINT_MAX)
        data_len = strlen((const char *)data);

    switch(e)
    {
    case MD4Hash:
        str_len = Weak1::MD4::DIGESTSIZE;
        h = new Weak1::MD4;
        break;
    case MD5Hash:
        str_len = Weak1::MD5::DIGESTSIZE;
        h = new Weak1::MD5;
        break;
    case SHA224Hash:
        str_len = SHA224::DIGESTSIZE;
        h = new SHA224;
        break;
    case SHA256Hash:
        str_len = SHA256::DIGESTSIZE;
        h = new SHA256;
        break;
    case SHA384Hash:
        str_len = SHA384::DIGESTSIZE;
        h = new SHA384;
        break;
    case SHA512Hash:
        str_len = SHA512::DIGESTSIZE;
        h = new SHA512;
        break;
    case SHA1Hash:
        str_len = SHA1::DIGESTSIZE;
        h = new SHA1;
        break;
    default:
        THROW_NEW_GUTIL_EXCEPTION(NotImplementedException);
        break;
    }


    String ret(str_len);
    ret.set_length(str_len);
    ret[str_len] = '\0';

    h->CalculateDigest(reinterpret_cast<byte *>(ret.Data()), data, data_len);
    return ret;
}

String EncryptionUtils::RandomString(GUINT32 num_bytes, GUINT32 seed)
{
    bool autoseed( seed == UINT_MAX );
    String ret(num_bytes);
    ret.set_length(num_bytes);

    AutoSeededX917RNG<AES> rng(false, autoseed);
    if(!autoseed)
        rng.IncorporateEntropy(reinterpret_cast<byte *>(&seed), sizeof(GUINT32));

    rng.GenerateBlock(reinterpret_cast<byte *>(ret.Data()), num_bytes);
    return ret;
}


END_NAMESPACE_GUTIL1;
