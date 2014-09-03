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
#include <cryptopp/ccm.h>
#include <cryptopp/aes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha3.h>
#include <math.h>
USING_NAMESPACE_GUTIL;

#define DEFAULT_CHUNK_SIZE 1024

namespace
{

// These are allocated at compile time, so consumers
//   of our class don't need to include cryptopp headers
struct d_t
{
    ::CryptoPP::AutoSeededX917RNG< ::CryptoPP::AES> rng;
    ::CryptoPP::CCM< ::CryptoPP::AES, GUtil::CryptoPP::Cryptor::TagLength>::Encryption enc;
    ::CryptoPP::CCM< ::CryptoPP::AES, GUtil::CryptoPP::Cryptor::TagLength>::Decryption dec;
};

}

NAMESPACE_GUTIL1(CryptoPP);


static void __compute_password_hash(byte *result, const char *password,
                                    const byte *salt, GUINT32 salt_len)
{
    ::CryptoPP::SHA3_256 h;
    if(salt)
        h.Update(salt, salt_len);
    h.Update((byte const *)password, password ? strlen(password) : 0);
    h.Final(result);
}

static void __compute_keyfile_hash(byte *result, const char *keyfile,
                                   const byte *salt, GUINT32 salt_len,
                                   IProgressHandler *ph = 0)
{
    SmartPointer<IInput> in;
    if(keyfile != NULL && strlen(keyfile) > 0){
        File *f = new File(keyfile);
        in = f;
        f->Open(File::OpenRead);
    }
    Hash<SHA3_256> h;
    if(salt)
        h.AddData(salt, salt_len);
    h.ComputeHash(result, in, DEFAULT_CHUNK_SIZE, ph);
}

static void __compute_password_hash2(byte *result, const char *password,
                                     const byte *salt, GUINT32 salt_len)
{
    ::CryptoPP::SHA3_512 h;
    if(salt)
        h.Update(salt, salt_len);
    h.Update((byte const *)password, password ? strlen(password) : 0);
    h.Final(result);
}


Cryptor::Cryptor(const char *password, const char *keyfile,
                 const byte *salt, GUINT32 salt_len)
    :m_nonceSize(DefaultNonceSize)
{
    ChangePassword(password, keyfile, salt, salt_len);
    G_D_INIT();
}

Cryptor::Cryptor(const Cryptor &other)
    :m_nonceSize(other.m_nonceSize)
{
    memcpy(m_key, other.m_key, sizeof(m_key));
    memcpy(m_key2, other.m_key2, sizeof(m_key2));
    G_D_INIT();
}

Cryptor::~Cryptor()
{
    G_D_UNINIT();
}

void Cryptor::SetNonceSize(GUINT8 size)
{
    if(size < 7 || 13 < size)
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Invalid nonce size");
    m_nonceSize = size;
}

bool Cryptor::CheckPassword(const char *password, const char *keyfile,
                            const byte *salt, GUINT32 salt_len) const
{
    byte buf_key[sizeof(m_key)] = {};
    byte buf_key2[sizeof(m_key2)] = {};
    if(keyfile == NULL || strlen(keyfile) == 0){
        // Only the password was given (or even a null password)
        __compute_password_hash(buf_key, password, salt, salt_len);
    }
    else{
        // A password and keyfile were given
        __compute_keyfile_hash(buf_key, keyfile, salt, salt_len);
    }
    __compute_password_hash2(buf_key2, password, salt, salt_len);
    return (0 == memcmp(m_key, buf_key, sizeof(buf_key))) &&
           (0 == memcmp(m_key2, buf_key2, sizeof(buf_key2)));
}

void Cryptor::ChangePassword(const char *password, const char *keyfile,
                             const byte *salt, GUINT32 salt_len)
{
    if(keyfile == NULL || strlen(keyfile) == 0){
        // Only the password was given (or even a null password)
        __compute_password_hash(m_key, password, salt, salt_len);
    }
    else{
        // A password and keyfile were given
        __compute_keyfile_hash(m_key, keyfile, salt, salt_len);
    }
    __compute_password_hash2(m_key2, password, salt, salt_len);
}


GUINT64 Cryptor::GetMaxPayloadLength() const
{
    return GetNonceSize() == 7 ? GUINT64_MAX : ((GUINT64)1 << 8*(15-GetNonceSize())) - 1;
}

void Cryptor::SetMaxPayloadLength(GUINT64 len)
{
    SetNonceSize(Min(13, 15 - ((FSB64(len) + 1) >> 3)));
}

void Cryptor::EncryptData(IOutput *out,
                          IInput *pData,
                          IInput *aData,
                          GUINT32 chunk_size,
                          IProgressHandler *ph)
{
    G_D;
    SmartArrayPointer<byte> n(new byte[GetNonceSize()]);
    if(ph) ph->ProgressUpdated(0);

    const GUINT64 aData_len = aData ? aData->BytesAvailable() : 0;
    GUINT64 len = pData ? pData->BytesAvailable() : 0;
    if(len == GUINT32_MAX)
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Source invalid: Length must be known");

    // With CCM the length is restricted, let's check that here
    if(GetMaxPayloadLength() < len)
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Payload too large for the given nonce size");

    // Initialize a random IV and initialize the encryptor
    d->rng.GenerateBlock(n, GetNonceSize());
    d->enc.SetKeyWithIV(m_key, sizeof(m_key), n.Data(), GetNonceSize());
    d->enc.SpecifyDataLengths(aData_len + sizeof(m_key2), len);

    GUINT32 read = 0;
    GUINT32 to_read = chunk_size == 0 ? len : chunk_size;
    GUINT32 buf_sz = to_read;
    ::CryptoPP::AuthenticatedEncryptionFilter ef(
                d->enc,
                new OutputInterfaceSink(*out),
                false,
                TagLength);

    // First pass the authenticated data. Our second key goes in here.
    ef.ChannelPut(::CryptoPP::AAD_CHANNEL, m_key2, sizeof(m_key2));

    // If they gave us additional auth data, add it here
    if(aData && 0 < aData->BytesAvailable())
    {
        SmartArrayPointer<byte> a(new byte[aData_len]);
        if(aData_len != aData->ReadBytes(a, aData_len, aData_len))
            THROW_NEW_GUTIL_EXCEPTION2(Exception, "Error reading from auth data source");
        ef.ChannelPut(::CryptoPP::AAD_CHANNEL, a.Data(), aData_len);
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
            if(out)
                out->Flush();
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
    out->WriteBytes(n, GetNonceSize());
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
    GUINT64 len;
    if(ph) ph->ProgressUpdated(0);
    const GUINT64 aData_len = aData ? aData->BytesAvailable() : 0;
    if(cData == NULL || (len = cData->BytesAvailable()) < (GetNonceSize() + TagLength))
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Invalid data length");
    len = len - (GetNonceSize() + TagLength);

    if(GetMaxPayloadLength() < len)
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Payload too large for the given nonce size");

    // Read the MAC tag and IV at the end of the crypttext
    SmartArrayPointer<byte> mac_iv(new byte[TagLength + GetNonceSize()]);
    cData->Seek(cData->Length() - (TagLength + GetNonceSize()));
    if(GetNonceSize() + TagLength !=
            cData->ReadBytes(mac_iv.Data(), GetNonceSize() + TagLength, GetNonceSize() + TagLength))
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Error reading from source");

    // Seek back to the start of the message
    cData->Seek(0);

    // Initialize the decryptor
    d->dec.SetKeyWithIV(m_key, sizeof(m_key), mac_iv.Data() + TagLength, GetNonceSize());
    d->dec.SpecifyDataLengths(aData_len + sizeof(m_key2), len);
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
        df.Put(mac_iv.Data(), TagLength);

        // Pass the authenticated data before the plaintext:
        df.ChannelPut(::CryptoPP::AAD_CHANNEL, m_key2, sizeof(m_key2));

        // If they gave us additional auth data, add it here
        if(aData && 0 < aData->BytesAvailable())
        {
            SmartArrayPointer<byte> a(new byte[aData_len]);
            if(aData_len != aData->ReadBytes(a, aData_len, aData_len))
                THROW_NEW_GUTIL_EXCEPTION2(Exception, "Error reading from auth data source");
            df.ChannelPut(::CryptoPP::AAD_CHANNEL, a.Data(), aData_len);
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
                if(out)
                    out->Flush();
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

double Cryptor::GetMaxKeyUsageSuggestion(GUINT8 nonce_length)
{
    double ret = 0;
    if(7 <= nonce_length && nonce_length <= 13){
        ret = pow(2, 8*nonce_length - 32);
    }
    return ret;
}


END_NAMESPACE_GUTIL1;
