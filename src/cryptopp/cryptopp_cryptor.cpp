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

#include "cryptopp_cryptor.h"
#include "cryptopp_sinks.h"
#include "gutil_d.h"
#include "gutil_exception.h"
#include "gutil_file.h"
#include "gutil_smartpointer.h"
#include "gutil_cryptopp_hash.h"
#include <cryptopp/gcm.h>
#include <cryptopp/aes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha3.h>
USING_NAMESPACE_GUTIL;

// These are some constants for the type of encryption we've chosen
#define KEYLENGTH   32

// The length of the second key, if we're using it
#define KEYLENGTH2  64

#define DEFAULT_CHUNK_SIZE 1024

namespace
{

// These are allocated at compile time, so consumers
//   of our class don't need to include cryptopp headers
struct d_t
{
    ::CryptoPP::AutoSeededX917RNG< ::CryptoPP::AES> rng;
    ::CryptoPP::GCM< ::CryptoPP::AES>::Encryption enc;
    ::CryptoPP::GCM< ::CryptoPP::AES>::Decryption dec;
    byte key[KEYLENGTH];
    byte key2[KEYLENGTH2];
};

}

NAMESPACE_GUTIL1(CryptoPP);


static void __compute_password_hash(byte result[KEYLENGTH], const char *password)
{
    ::CryptoPP::SHA3_256().CalculateDigest(result, (byte const *)password, password ? strlen(password) : 0);
}

static void __compute_keyfile_hash(byte result[KEYLENGTH], const char *keyfile, IProgressHandler *ph = 0)
{
    SmartPointer<IInput> in;
    if(keyfile != NULL && strlen(keyfile) > 0){
        File *f = new File(keyfile);
        f->Open(File::OpenRead);
        in = f;
    }
    ::GUtil::CryptoPP::Hash<SHA3_256>().ComputeHash(result, in, DEFAULT_CHUNK_SIZE, ph);
}

static void __compute_password_hash2(byte result[KEYLENGTH2], const char *password)
{
    ::CryptoPP::SHA3_512().CalculateDigest(result, (byte const *)password, password ? strlen(password) : 0);
}


Cryptor::Cryptor(const char *password, const char *keyfile)
{
    G_D_INIT();
    ChangePassword(password, keyfile);
}

Cryptor::Cryptor(const Cryptor &other)
{
    G_D_INIT();
    G_D;
    memcpy(d->key, ((d_t *)other.d)->key, KEYLENGTH);
    memcpy(d->key2, ((d_t *)other.d)->key2, KEYLENGTH2);
}

Cryptor::~Cryptor()
{
    G_D_UNINIT();
}

bool Cryptor::CheckPassword(const char *password, const char *keyfile) const
{
    G_D;
    byte buf_key[KEYLENGTH] = {};
    byte buf_key2[KEYLENGTH2] = {};
    if(keyfile == NULL || strlen(keyfile) == 0){
        // Only the password was given (or even a null password)
        __compute_password_hash(buf_key, password);
    }
    else{
        // A password and keyfile were given
        __compute_keyfile_hash(buf_key, keyfile);
    }
    __compute_password_hash2(buf_key2, password);
    return (0 == memcmp(d->key, buf_key, sizeof(buf_key))) &&
           (0 == memcmp(d->key2, buf_key2, sizeof(buf_key2)));
}

void Cryptor::ChangePassword(const char *password, const char *keyfile)
{
    G_D;
    if(keyfile == NULL || strlen(keyfile) == 0){
        // Only the password was given (or even a null password)
        __compute_password_hash(d->key, password);
    }
    else{
        // A password and keyfile were given
        __compute_keyfile_hash(d->key, keyfile);
    }
    __compute_password_hash2(d->key2, password);
}


void Cryptor::EncryptData(IOutput *out,
                          IInput *pData,
                          IInput *aData,
                          GUINT32 chunk_size,
                          IProgressHandler *ph)
{
    G_D;
    byte iv[IVLength];
    if(ph) ph->ProgressUpdated(0);

    // Initialize a random IV and initialize the encryptor
    d->rng.GenerateBlock(iv, sizeof(iv));
    d->enc.SetKeyWithIV(d->key, KEYLENGTH, iv, IVLength);

    GUINT32 len = pData ? pData->BytesAvailable() : 0;
    if(len == GUINT32_MAX)
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Source invalid: Length must be known");

    GUINT32 read = 0;
    GUINT32 to_read = chunk_size == 0 ? len : chunk_size;
    GUINT32 buf_sz = to_read;
    ::CryptoPP::AuthenticatedEncryptionFilter ef(
                d->enc,
                new OutputInterfaceSink(*out),
                false,
                TagLength);

    // First pass the authenticated data. Our second key goes in here.
    ef.ChannelPut(::CryptoPP::AAD_CHANNEL, d->key2, KEYLENGTH2);

    // If they gave us additional auth data, add it here
    if(aData && 0 < aData->BytesAvailable())
    {
        const GUINT32 len = aData->BytesAvailable();
        SmartArrayPointer<byte> a(new byte[len]);
        if(len != aData->ReadBytes(a, len, len))
            THROW_NEW_GUTIL_EXCEPTION2(Exception, "Error reading from auth data source");
        ef.ChannelPut(::CryptoPP::AAD_CHANNEL, a.Data(), len);
    }
    ef.ChannelMessageEnd(::CryptoPP::AAD_CHANNEL);

    // Then read and pass the plaintext data
    if(read < len)
    {
        SmartArrayPointer<byte> buf(new byte[buf_sz]);
        while(read < len)
        {
            GASSERT(pData);

            if((read + to_read) > len)
                to_read = len - read;

            if(to_read != pData->ReadBytes(buf, buf_sz, to_read))
                THROW_NEW_GUTIL_EXCEPTION2(Exception, "Error reading from source");

            ef.Put(buf, to_read, true);
            read += to_read;

            if(ph){
                ph->ProgressUpdated(((float)read / len) * 100);
                if(ph->ShouldOperationCancel())
                    THROW_NEW_GUTIL_EXCEPTION(CancelledOperationException);
            }
        }
    }
    ef.MessageEnd();

    // Write the IV at the very end
    out->WriteBytes(iv, sizeof(iv));
    out->Flush();
    if(ph) ph->ProgressUpdated(100);
}


void Cryptor::DecryptData(IOutput *out,
                          IRandomAccessInput *cData,
                          IInput *aData,
                          GUINT32 chunk_size,
                          IProgressHandler *ph)
{
    G_D;
    GUINT32 len;
    if(cData == NULL || (len = cData->BytesAvailable()) < (IVLength + TagLength))
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Invalid data length");
    len = len - (IVLength + TagLength);

    // Read the MAC tag and IV at the end of the crypttext
    byte mac_iv[TagLength + IVLength];
    cData->Seek(cData->Length() - sizeof(mac_iv));
    if(sizeof(mac_iv) != cData->ReadBytes(mac_iv, sizeof(mac_iv), sizeof(mac_iv)))
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Error reading from source");

    // Seek back to the start of the message
    cData->Seek(0);

    // Initialize the decryptor
    d->dec.SetKeyWithIV(d->key, KEYLENGTH, mac_iv + TagLength, IVLength);
    try
    {
        GUINT32 read = 0;
        GUINT32 to_read = chunk_size == 0 ? len : chunk_size;
        GUINT32 buf_sz = to_read;
        ::CryptoPP::AuthenticatedDecryptionFilter df(
                    d->dec,
                    out == NULL ? NULL : new OutputInterfaceSink(*out),
                    ::CryptoPP::AuthenticatedDecryptionFilter::THROW_EXCEPTION |
                    ::CryptoPP::AuthenticatedDecryptionFilter::MAC_AT_BEGIN,
                    TagLength);

        // We have to give it the mac before everything else in order to decrypt both
        //  the pData and the aData
        df.Put(mac_iv, TagLength);

        // Pass the authenticated data before the plaintext:
        df.ChannelPut(::CryptoPP::AAD_CHANNEL, d->key2, KEYLENGTH2);

        // If they gave us additional auth data, add it here
        if(aData && 0 < aData->BytesAvailable())
        {
            const GUINT32 len = aData->BytesAvailable();
            SmartArrayPointer<byte> a(new byte[len]);
            if(len != aData->ReadBytes(a, len, len))
                THROW_NEW_GUTIL_EXCEPTION2(Exception, "Error reading from auth data source");
            df.ChannelPut(::CryptoPP::AAD_CHANNEL, a.Data(), len);
        }
        df.ChannelMessageEnd(::CryptoPP::AAD_CHANNEL);

        // Then read and pass the crypttext
        if(read < len)
        {
            SmartArrayPointer<byte> buf(new byte[buf_sz]);
            while(read < len)
            {
                if((read + to_read) > len)
                    to_read = len - read;

                if(to_read != cData->ReadBytes(buf, buf_sz, to_read))
                    THROW_NEW_GUTIL_EXCEPTION2(Exception, "Error reading from source");

                df.Put(buf, to_read, true);
                read += to_read;

                if(ph){
                    ph->ProgressUpdated(((float)read / len) * 100);
                    if(ph->ShouldOperationCancel())
                        THROW_NEW_GUTIL_EXCEPTION(CancelledOperationException);
                }
            }
        }

        // This will throw an exception if authenticity validation failed
        df.MessageEnd();
    }
    catch(const ::CryptoPP::Exception &ex)
    {
        THROW_NEW_GUTIL_EXCEPTION2(AuthenticationException, ex.what());
    }
    if(out) out->Flush();
    if(ph) ph->ProgressUpdated(100);
}


END_NAMESPACE_GUTIL1;
