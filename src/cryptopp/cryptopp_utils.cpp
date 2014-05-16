/*Copyright 2010-2013 George Karagoulis

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

#include "cryptopp_utils.h"
#include "gutil_smartpointer.h"
#include "cryptopp/cryptlib.h"
#include "cryptopp/filters.h"
#include "cryptopp/gzip.h"
#include "cryptopp/randpool.h"
#include "cryptopp/osrng.h"
#include "cryptopp/default.h"
#include "cryptopp/pwdbased.h"
#include "cryptopp/md4.h"
#include "cryptopp/md5.h"
#include "cryptopp/simple.h"
using namespace CryptoPP;


/** Used to adapt the GUtil String into CryptoPP Sink. */
class GUtil_StringSink :
        public Bufferless<Sink>
{
    GUtil::String &sref;
public:

    /** Just give it a string reference, and it will append all the data to it */
    GUtil_StringSink(GUtil::String &s) :sref(s){}

    /** Overridden from ::CryptoPP::Sink*/
    virtual size_t Put2(const byte *inString, size_t length, int, bool){
        sref.Append(reinterpret_cast<const char *>(inString), length);
        return 0;
    }

};


NAMESPACE_GUTIL;


String Crypto::CompressString(const GBYTE *data, GINT32 data_len,
                                       Crypto::CompressionLevelEnum level)
{
    String ret(data_len + 1);   // Preallocate space for the return string
    bool skip_compression = data_len > 10000000;

    if(!skip_compression)
    {
        if(level < MinimumCompression || level > MaximumCompression)
            level = DefaultCompression;

        Gzip zipper(new GUtil_StringSink(ret), level);
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

String Crypto::DecompressString(const GBYTE *data, GINT32 data_len)
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
            StringSource(start, data_len, true, new Gunzip(new GUtil_StringSink(ret)));
        }
        else
        {
            ret.Append((const char *)start, data_len);
        }
    }

    return ret;
}


#define ENCRYPTED_MESSAGE_STAMP 0xAAAAAAAAAAAAAAAAULL

String Crypto::EncryptString(const GBYTE *data, GINT32 data_len,
                                      const GBYTE *key, GINT32 len, EncryptionTypeEnum e)
{
    byte padded_key[SHA256::DIGESTSIZE];
    String ret(data_len);   // A heuristic to estimate the length of the return string
    if(len == INT_MAX)
        len = strlen(reinterpret_cast<const char *>(key));

    SmartPointer<StreamTransformation> mode;

    // At least as big as the largest block size
    byte init_vec[AES::BLOCKSIZE];
    GINT32 block_size;

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

    StreamTransformationFilter enc(*mode, new GUtil_StringSink(ret));

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

String Crypto::DecryptString(const GBYTE *data, GINT32 data_len, const GBYTE *key, GINT32 len, EncryptionTypeEnum e)
{
    String ret(data_len);
    if(len == INT_MAX)
        len = strlen(reinterpret_cast<const char *>(key));


    // Each encryption algorithm may use a different block size, and therefore require
    //  a different length initialization vector
    GINT32 iv_len;

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

    StreamTransformationFilter decryptor(*mode, new GUtil_StringSink(ret));
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

String Crypto::Hash(const GBYTE *data, GINT32 data_len, HashTypeEnum e)
{
    GINT32 str_len;
    SmartPointer<HashTransformation> h;

    if(data_len == -1)
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

    String ret((char)0, str_len);
    h->CalculateDigest((byte *)ret.Data(), data, data_len);
    return ret;
}


END_NAMESPACE_GUTIL;
