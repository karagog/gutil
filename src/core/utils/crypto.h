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

NAMESPACE_GUTIL;


/** A static class to encapsulate certain cryptographic functionality
*/
class Crypto
{
public:


    /** Used to adapt the GUtil String into CryptoPP Sink. */
    class StringSink :
            public ::CryptoPP::Bufferless< ::CryptoPP::Sink >
    {
        GUtil::String &sref;
    public:

        /** Just give it a string reference, and it will append all the data to it */
        StringSink(GUtil::String &s) :sref(s){}

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
    static String EncryptString(const String &str,
                                                    const String &key,
                                                    EncryptionTypeEnum e = DefaultEncryption){
        return EncryptString((const GBYTE *)str.ConstData(), str.Length(), key, e);
    }
    /** Returns a decrypted copy of the data, using the given string as an encryption string */
    static String DecryptString(const String &str,
                                                    const String &key,
                                                    EncryptionTypeEnum e = DefaultEncryption){
        return DecryptString((const GBYTE *)str.ConstData(), str.Length(), key, e);
    }

    /** Returns an encrypted copy of the data, using the given string as an encryption string */
    static String EncryptString(const GBYTE *data, GINT32 length,
                                                    const String &key,
                                                    EncryptionTypeEnum e = DefaultEncryption){
        return EncryptString(data, length, (const GBYTE *)key.ConstData(), key.Length(), e);
    }
    /** Returns a decrypted copy of the data, using the given string as an encryption string */
    static String DecryptString(const GBYTE *data, GINT32 length,
                                                    const String &key,
                                                    EncryptionTypeEnum e = DefaultEncryption){
        return DecryptString(data, length, (const GBYTE *)key.ConstData(), key.Length(), e);
    }

    /** Returns an encrypted copy of the data, using the given string as an encryption string */
    static String EncryptString(const GBYTE *data, GINT32 data_len,
                                             const GBYTE *key, GINT32 key_len,
                                             EncryptionTypeEnum e = DefaultEncryption);
    /** Returns a decrypted copy of the data, using the given string as an encryption string */
    static String DecryptString(const GBYTE *data, GINT32 data_len,
                                             const GBYTE *key, GINT32 keylen,
                                             EncryptionTypeEnum e = DefaultEncryption);





    /** Returns a zip-compressed version of the string.
        \note Depending on the string's contents, you may not be able to compress it,
        in which case the string stays roughly the same size.  The function is optimized to
        recognize when the string actually grows from compression, and doesn't compress it
        in this case.
    */
    static String CompressString(const String &data, CompressionLevelEnum c = DefaultCompression){
        return CompressString((const GBYTE *)data.ConstData(), data.Length(), c);
    }
    /** Returns a copy of the compressed string after it has been inflated */
    static String DecompressString(const String &data){
        return DecompressString((const GBYTE *)data.ConstData(), data.Length());
    }

    /** Returns a zip-compressed version of the string.
        \note Depending on the string's contents, you may not be able to compress it,
        in which case the string stays roughly the same size.  The function is optimized to
        recognize when the string actually grows from compression, and doesn't compress it
        in this case.
    */
    static String CompressString(const GBYTE *data, GINT32 data_len,
                                                     CompressionLevelEnum c = DefaultCompression);
    /** Returns a copy of the compressed string after it has been inflated */
    static String DecompressString(const GBYTE *data, GINT32 data_len);




    /** Returns a hash of the string */
    static String Hash(const String &data, HashAlgorithmEnum e = DefaultHash){
        return Hash((const GBYTE *)data.ConstData(), data.Length(), e);
    }
    /** Returns a hash of the provided data */
    static String Hash(const char *data, GINT32 data_len = INT_MAX, HashAlgorithmEnum e = DefaultHash){
        return Hash((const GBYTE *)data, data_len, e);
    }
    /** Returns a hash of the provided data */
    static String Hash(const GBYTE *data, GINT32 data_len = INT_MAX, HashAlgorithmEnum e = DefaultHash);


    /** Returns a string of random data.  You can optionally supply a seed value, otherwise
        it will be auto-seeded (implementation in CryptoPP library).
    */
    static String RandomString(GINT32 num_bytes, GINT32 seed = INT_MAX);

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_CRYPTO_H

#endif // GUTIL_NO_CRYPTOPP
