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
USING_NAMESPACE_GUTIL;

// These are some constants for the type of encryption we've chosen
#define KEYLENGTH   32

// The recommended IV size for GCM is 96 bits (NIST SP-800-38D)
#define IVLENGTH    12

// The default tag length for GCM is 128 bits, but we'll explicitly declare it here
#define TAGLENGTH   16

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
    byte auth_data[KEYLENGTH];
};

}

NAMESPACE_GUTIL1(CryptoPP);


static void __compute_password_hash(byte result[KEYLENGTH], const char *password)
{
    const int len = password ? strlen(password) : 0;
    Hash<SHA3_256>().ComputeHash(result, (byte const *)password, len);
}

static void __compute_keyfile_hash(byte result[KEYLENGTH], const char *keyfile, IProgressHandler *ph = 0)
{
    SmartPointer<IInput> in;
    if(keyfile != NULL && strlen(keyfile) > 0){
        File *f = new File(keyfile);
        f->Open(File::OpenRead);
        in = f;
    }

    Hash<SHA3_256> hash;
    hash.ComputeHash(result, in, DEFAULT_CHUNK_SIZE, ph);
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
    memcpy(d->key, ((d_t *)other.d)->key, sizeof(d->key));
    memcpy(d->auth_data, ((d_t *)other.d)->auth_data, sizeof(d->auth_data));
}

Cryptor::~Cryptor()
{
    G_D_UNINIT();
}

bool Cryptor::CheckPassword(const char *password, const char *keyfile) const
{
    G_D;
    byte buf_key[sizeof(d->key)] = {};
    byte buf_auth[sizeof(d->auth_data)] = {};
    if(keyfile == NULL || strlen(keyfile) == 0){
        // Only the password was given (or even a null password)
        __compute_password_hash(buf_key, password);
        memcpy(buf_auth, buf_key, sizeof(buf_key));
    }
    else if(password == NULL || strlen(password) == 0){
        // A keyfile was given but no password
        __compute_keyfile_hash(buf_key, keyfile);
        memcpy(buf_auth, buf_key, sizeof(buf_key));
    }
    else{
        // A password and keyfile were given
        __compute_keyfile_hash(buf_key, keyfile);
        __compute_password_hash(buf_auth, password);
    }
    return (0 == memcmp(d->key, buf_key, sizeof(buf_key))) &&
           (0 == memcmp(d->auth_data, buf_auth, sizeof(buf_auth)));
}

void Cryptor::ChangePassword(const char *password, const char *keyfile)
{
    G_D;
    if(keyfile == NULL || strlen(keyfile) == 0){
        // Only the password was given (or even a null password)
        __compute_password_hash(d->key, password);
        memcpy(d->auth_data, d->key, sizeof(d->key));
    }
    else if(password == NULL || strlen(password) == 0){
        // A keyfile was given but no password
        __compute_keyfile_hash(d->key, keyfile);
        memcpy(d->auth_data, d->key, sizeof(d->key));
    }
    else{
        // A password and keyfile were given
        __compute_keyfile_hash(d->key, keyfile);
        __compute_password_hash(d->auth_data, password);
    }
}


void Cryptor::EncryptData(IOutput *out,
                          IInput *pData,
                          IInput *aData,
                          GUINT32 chunk_size,
                          IProgressHandler *ph) const
{
    G_D;
    byte iv[IVLENGTH];
    if(ph) ph->ProgressUpdated(0);

    // Initialize a random IV and initialize the encryptor
    d->rng.GenerateBlock(iv, sizeof(iv));
    d->enc.SetKeyWithIV(d->key, KEYLENGTH, iv, IVLENGTH);

    // First write the IV
    out->WriteBytes(iv, sizeof(iv));

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
                TAGLENGTH);

    // First pass the authenticated data:
    ef.ChannelPut(::CryptoPP::AAD_CHANNEL, d->auth_data, sizeof(d->auth_data));

    // If they gave us additional auth data, add it here
    if(aData && 0 < aData->BytesAvailable())
    {
        SmartArrayPointer<byte> a(new byte[aData->BytesAvailable()]);
        aData->ReadBytes(a, aData->BytesAvailable(), aData->BytesAvailable());
        ef.ChannelPut(::CryptoPP::AAD_CHANNEL, a.Data(), aData->BytesAvailable());
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
                if(ph->CancelOperation())
                    THROW_NEW_GUTIL_EXCEPTION(CancelledOperationException);
            }
        }
    }
    ef.MessageEnd();
    out->Flush();
    if(ph) ph->ProgressUpdated(100);
}


void Cryptor::DecryptData(IOutput *out,
                          IRandomAccessInput *cData,
                          IInput *aData,
                          GUINT32 chunk_size,
                          IProgressHandler *ph) const
{
    G_D;
    GUINT32 len;
    if((len = cData->BytesAvailable()) < (IVLENGTH + TAGLENGTH))
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Invalid data length");
    len = len - (IVLENGTH + TAGLENGTH);

    // Read the IV from the start of the source
    byte iv[IVLENGTH];
    if(IVLENGTH != cData->ReadBytes(iv, sizeof(iv), IVLENGTH))
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Error reading from source");

    // Read the MAC tag at the end of the crypttext
    byte mac[TAGLENGTH];
    const GUINT32 cData_startPos = cData->Pos();
    cData->Seek(cData->Length() - TAGLENGTH);
    if(TAGLENGTH != cData->ReadBytes(mac, TAGLENGTH, TAGLENGTH))
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Error reading from source");

    // Seek back to the start of the message
    cData->Seek(cData_startPos);

    // Initialize the decryptor
    d->dec.SetKeyWithIV(d->key, KEYLENGTH, iv, IVLENGTH);
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
                    TAGLENGTH);

        // We have to give it the mac before everything else in order to decrypt both
        //  the pData and the aData
        df.Put(mac, TAGLENGTH);

        // Pass the authenticated data before the plaintext:
        df.ChannelPut(::CryptoPP::AAD_CHANNEL, d->auth_data, sizeof(d->auth_data));

        // If they gave us additional auth data, add it here
        if(aData && 0 < aData->BytesAvailable())
        {
            SmartArrayPointer<byte> a(new byte[aData->BytesAvailable()]);
            aData->ReadBytes(a, aData->BytesAvailable(), aData->BytesAvailable());
            df.ChannelPut(::CryptoPP::AAD_CHANNEL, a.Data(), aData->BytesAvailable());
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
                    if(ph->CancelOperation())
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
