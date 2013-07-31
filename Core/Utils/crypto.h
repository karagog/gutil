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

/** \file

    Describes the GUtil encryption functionality.

    By default, most functionality is implemented by calls to Crypto++.  You can
    disable this with the build switch GUTIL_NO_CRYPTOPP.
*/


#ifndef GUTIL_NO_CRYPTOPP

#ifndef GUTIL_CRYPTO_H
#define GUTIL_CRYPTO_H

#include "gutil_strings.h"
#include "cryptopp/simple.h"

NAMESPACE_GUTIL1(Utils);


/** A static class to encapsulate certain cryptographic functionality
*/
class Crypto
{
public:


    /** Used to adapt the GUtil String into CryptoPP Sink. */
    class StringSink :
            public ::CryptoPP::Bufferless< ::CryptoPP::Sink >
    {
        GUtil::DataObjects::String &sref;
    public:

        /** Just give it a string reference, and it will append all the data to it */
        inline StringSink(GUtil::DataObjects::String &s) :sref(s){}

        /** Overridden from ::CryptoPP::Sink*/
        virtual size_t Put2(const byte *inString, size_t length, int, bool){
            sref.Append(reinterpret_cast<const char *>(inString), length);
            return 0;
        }

    };


    /** Declares various types of encryption methods for use in the encryption function. */
    enum EncryptionTypeEnum
    {
        /** The default encryption method (AES). */
        DefaultEncryption,

        /** The most solid form of encryption (Advanced Encryption Standard is the current
            encryption standard approved by the NSA).
        */
        AES_Encryption,

        /** DES-EDE3, also known as Triple-DES. */
        TripleDES_Encryption,

        /** The weakest form of encryption provided. */
        DES_Encryption
    };


    /** Defines levels of compression for the Compress() function.
        \note These values correspond to the Gzip deflate levels in CryptoPP, but their values
        are not actually tied to the ones in CryptoPP, so if for some reason CryptoPP adds/removes
        deflate levels, it will have to be adjusted here so it matches.
    */
    enum CompressionLevelEnum
    {
        DefaultCompression = 6,
        MinimumCompression = 0,
        MaximumCompression = 9
    };


    /** Describes different hash algorithms. */
    enum HashAlgorithmEnum
    {
        DefaultHash,
        SHA224Hash,
        SHA256Hash,
        SHA384Hash,
        SHA512Hash,
        SHA1Hash,
        MD5Hash,
        MD4Hash
    };


    /** Returns an encrypted copy of the data, using the given string as an encryption string */
    inline static DataObjects::String EncryptString(const DataObjects::String &str,
                                                    const DataObjects::String &key,
                                                    EncryptionTypeEnum e = DefaultEncryption){
        return EncryptString((const GBYTE *)str.ConstData(), str.Length(), key, e);
    }
    /** Returns a decrypted copy of the data, using the given string as an encryption string */
    inline static DataObjects::String DecryptString(const DataObjects::String &str,
                                                    const DataObjects::String &key,
                                                    EncryptionTypeEnum e = DefaultEncryption){
        return DecryptString((const GBYTE *)str.ConstData(), str.Length(), key, e);
    }

    /** Returns an encrypted copy of the data, using the given string as an encryption string */
    inline static DataObjects::String EncryptString(const GBYTE *data, GUINT32 length,
                                                    const DataObjects::String &key,
                                                    EncryptionTypeEnum e = DefaultEncryption){
        return EncryptString(data, length, (const GBYTE *)key.ConstData(), key.Length(), e);
    }
    /** Returns a decrypted copy of the data, using the given string as an encryption string */
    inline static DataObjects::String DecryptString(const GBYTE *data, GUINT32 length,
                                                    const DataObjects::String &key,
                                                    EncryptionTypeEnum e = DefaultEncryption){
        return DecryptString(data, length, (const GBYTE *)key.ConstData(), key.Length(), e);
    }

    /** Returns an encrypted copy of the data, using the given string as an encryption string */
    static DataObjects::String EncryptString(const GBYTE *data, GUINT32 data_len,
                                             const GBYTE *key, GUINT32 key_len,
                                             EncryptionTypeEnum e = DefaultEncryption);
    /** Returns a decrypted copy of the data, using the given string as an encryption string */
    static DataObjects::String DecryptString(const GBYTE *data, GUINT32 data_len,
                                             const GBYTE *key, GUINT32 keylen,
                                             EncryptionTypeEnum e = DefaultEncryption);





    /** Returns a zip-compressed version of the string.
        \note Depending on the string's contents, you may not be able to compress it,
        in which case the string stays roughly the same size.  The function is optimized to
        recognize when the string actually grows from compression, and doesn't compress it
        in this case.
    */
    inline static DataObjects::String CompressString(const DataObjects::String &data, CompressionLevelEnum c = DefaultCompression){
        return CompressString((const GBYTE *)data.ConstData(), data.Length(), c);
    }
    /** Returns a copy of the compressed string after it has been inflated */
    inline static DataObjects::String DecompressString(const DataObjects::String &data){
        return DecompressString((const GBYTE *)data.ConstData(), data.Length());
    }

    /** Returns a zip-compressed version of the string.
        \note Depending on the string's contents, you may not be able to compress it,
        in which case the string stays roughly the same size.  The function is optimized to
        recognize when the string actually grows from compression, and doesn't compress it
        in this case.
    */
    static DataObjects::String CompressString(const GBYTE *data, GUINT32 data_len,
                                                     CompressionLevelEnum c = DefaultCompression);
    /** Returns a copy of the compressed string after it has been inflated */
    static DataObjects::String DecompressString(const GBYTE *data, GUINT32 data_len);




    /** Returns a hash of the string */
    inline static DataObjects::String Hash(const DataObjects::String &data, HashAlgorithmEnum e = DefaultHash){
        return Hash((const GBYTE *)data.ConstData(), data.Length(), e);
    }
    /** Returns a hash of the provided data */
    inline static DataObjects::String Hash(const char *data, GUINT32 data_len = UINT_MAX, HashAlgorithmEnum e = DefaultHash){
        return Hash((const GBYTE *)data, data_len, e);
    }
    /** Returns a hash of the provided data */
    static DataObjects::String Hash(const GBYTE *data, GUINT32 data_len = UINT_MAX, HashAlgorithmEnum e = DefaultHash);


    /** Returns a string of random data.  You can optionally supply a seed value, otherwise
        it will be auto-seeded (implementation in CryptoPP library).
    */
    static DataObjects::String RandomString(GUINT32 num_bytes, GUINT32 seed = UINT_MAX);

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_CRYPTO_H

#endif // GUTIL_NO_CRYPTOPP
