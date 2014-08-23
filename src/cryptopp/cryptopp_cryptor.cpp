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
#include <cryptopp/gcm.h>
#include <cryptopp/aes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>
using namespace CryptoPP;

// These are some constants for the type of encryption we've chosen
#define KEYLENGTH   32

// The recommended IV size for GCM is 96 bits (NIST SP-800-38D)
#define IVLENGTH    12

// The default tag length for GCM is 128 bits, but we'll explicitly declare it here
#define TAGLENGTH   16

namespace
{

// These are allocated at compile time, so consumers
//   of our class don't need to include cryptopp headers
struct d_t
{
    AutoSeededX917RNG<AES> rng;
    GCM<AES>::Encryption enc;
    GCM<AES>::Decryption dec;
    byte key[KEYLENGTH];
};

}

NAMESPACE_GUTIL;


static void __compute_password_hash(byte result[KEYLENGTH], const char *password, byte const *salt, GUINT32 saltLen)
{
    SHA256 hash;
    if(salt && 0 < saltLen)
        hash.Update(salt, saltLen);
    hash.Update((byte const *)password, strlen(password));
    hash.Final(result);
}


Cryptor::Cryptor(const char *password, byte const *salt, GUINT32 saltLen)
{
    G_D_INIT();
    ChangePassword(password, salt, saltLen);
}

Cryptor::Cryptor(const Cryptor &other)
{
    G_D_INIT();
    G_D;
    memcpy(d->key, ((d_t *)other.d)->key, sizeof(d->key));
}

Cryptor::~Cryptor()
{
    G_D_UNINIT();
}

bool Cryptor::CheckPassword(const char *password, byte const *salt, GUINT32 saltLen) const
{
    G_D;
    byte buf[KEYLENGTH];
    __compute_password_hash(buf, password, salt, saltLen);
    return 0 == memcmp(d->key, buf, KEYLENGTH);
}

void Cryptor::ChangePassword(const char *password, byte const *salt, GUINT32 saltLen)
{
    G_D;
    __compute_password_hash(d->key, password, salt, saltLen);
}


void Cryptor::EncryptData(InputInterface *pData,
                          OutputInterface *out,
                          InputInterface *aData,
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
    AuthenticatedEncryptionFilter ef(d->enc, new OutputInterfaceSink(*out), false, TAGLENGTH);

    // First pass the authenticated data:
    if(aData && 0 < aData->BytesAvailable())
    {
        SmartArrayPointer<byte> a(new byte[aData->BytesAvailable()]);
        aData->ReadBytes(a, aData->BytesAvailable(), aData->BytesAvailable());

        ef.ChannelPut(AAD_CHANNEL, a.Data(), aData->BytesAvailable());
        ef.ChannelMessageEnd(AAD_CHANNEL);
    }

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
}


void Cryptor::DecryptData(InputInterface *cData,
                          OutputInterface *out,
                          InputInterface *aData,
                          GUINT32 chunk_size,
                          IProgressHandler *ph) const
{
    G_D;
    GUINT32 len;
    if((len = cData->BytesAvailable()) < (IVLENGTH + TAGLENGTH))
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Invalid data length");
    len = len - IVLENGTH;

    // Read the IV from the start of the source
    byte iv[IVLENGTH];
    if(IVLENGTH != cData->ReadBytes(iv, sizeof(iv), IVLENGTH))
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Error reading from source");

    // Initialize the decryptor
    d->dec.SetKeyWithIV(d->key, KEYLENGTH, iv, IVLENGTH);
    try
    {
        GUINT32 read = 0;
        GUINT32 to_read = chunk_size == 0 ? len : chunk_size;
        GUINT32 buf_sz = to_read;
        AuthenticatedDecryptionFilter df(d->dec,
                       out == NULL ? NULL : new OutputInterfaceSink(*out),
                       AuthenticatedDecryptionFilter::THROW_EXCEPTION,
                       TAGLENGTH);

        // First pass the authenticated data:
        if(aData && 0 < aData->BytesAvailable())
        {
            SmartArrayPointer<byte> a(new byte[aData->BytesAvailable()]);
            aData->ReadBytes(a, aData->BytesAvailable(), aData->BytesAvailable());

            df.ChannelPut(AAD_CHANNEL, a.Data(), aData->BytesAvailable());
            df.ChannelMessageEnd(AAD_CHANNEL);
        }

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

        // This will throw an exception if validation of authenticity failed
        df.MessageEnd();
    }
    catch(const CryptoPP::Exception &ex)
    {
        THROW_NEW_GUTIL_EXCEPTION2(AuthenticationException, ex.what());
    }
    if(out) out->Flush();
}


END_NAMESPACE_GUTIL;
