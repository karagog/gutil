/*Copyright 2014-2015 George Karagoulis

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
#include <gutil/d.h>
#include <gutil/exception.h>
#include <gutil/file.h>
#include <gutil/smartpointer.h>
#include <gutil/cryptopp_hash.h>
#include <cryptopp/ccm.h>
#include <cryptopp/aes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha3.h>
#include <math.h>
USING_NAMESPACE_GUTIL;
using namespace std;

// The number of iterations for our key derivation function
#define KDF_ITERATIONS (((GUINT32)1)<<16)

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


// This needs to be defined or we get linker errors since this is used in the code
//  in places other than a const expression
const GUINT32 Cryptor::TagLength;


Cryptor::Cryptor(const Credentials &creds,
                 GUINT8 nonce_size,
                 IKeyDerivation *kdf)
    :m_nonceSize(nonce_size),
      m_kdf(kdf)
{
    if(m_nonceSize < 7 || 13 < m_nonceSize)
        throw Exception<>("Invalid nonce size");

    ChangeCredentials(creds);
    G_D_INIT();
}

Cryptor::Cryptor(const Cryptor &other)
    :m_nonceSize(other.m_nonceSize),
      m_authData(other.m_authData),
      m_kdf(static_cast<IKeyDerivation *>(other.m_kdf->Clone())),
      m_credsType(other.m_credsType)
{
    memcpy(m_key, other.m_key, sizeof(m_key));
    G_D_INIT();
}

Cryptor::~Cryptor()
{
    G_D_UNINIT();
}

bool Cryptor::CheckCredentials(const Credentials &creds) const
{
    byte buf_key[sizeof(m_key)];
    const Vector<byte> tmpauth = m_kdf->DeriveAuthData(creds.Password);
    bool success = true;
    try{
        if(Credentials::PasswordType == creds.Type){
            // Only the password was given (or even a null password)
            m_kdf->DeriveKeyFromPassword(buf_key, creds.Password);
        }
        else{
            // A keyfile was given (and optional password)
            m_kdf->DeriveKeyFromKeyfile(buf_key, creds.Keyfile);

        }
    } catch(...) {
        success = false;
    }
    return success &&
            tmpauth.Length() == m_authData.Length() &&
            0 == memcmp(m_key, buf_key, sizeof(buf_key)) &&
            0 == memcmp(m_authData.ConstData(), tmpauth.ConstData(), m_authData.Length());
}

void Cryptor::ChangeCredentials(const Credentials &creds)
{
    if(Credentials::PasswordType == creds.Type){
        // Only the password was given (or even a null password)
        m_kdf->DeriveKeyFromPassword(m_key, creds.Password);
    }
    else{
        // A keyfile was given (and optionally a password)
        m_kdf->DeriveKeyFromKeyfile(m_key, creds.Keyfile);
    }
    m_authData = m_kdf->DeriveAuthData(creds.Password);
    m_credsType = creds.Type;
}


GUINT64 Cryptor::GetMaxPayloadLength(GUINT8 nonce_size)
{
    return nonce_size == 7 ? GUINT64_MAX : ((GUINT64)1 << 8*(15-nonce_size)) - 1;
}

GUINT8 Cryptor::GetNonceSizeRequired(GUINT64 len)
{
    return Min(13, 15 - ((FSB64(len) + 1) >> 3));
}

void Cryptor::EncryptData(IOutput *out,
                          IInput *pData,
                          IInput *aData,
                          byte const *nonce,
                          GUINT32 chunk_size,
                          function<bool(int)> progress_cb)
{
    G_D;
    SmartArrayPointer<byte> n;
    if(NULL == nonce)
        n.Set(new byte[GetNonceSize()]);
    progress_cb(0);
    finally([=]{ progress_cb(100); });

    const GUINT64 aData_len = aData ? aData->BytesAvailable() : 0;
    GUINT64 len = pData ? pData->BytesAvailable() : 0;
    if(len == GUINT32_MAX)
        throw Exception<>("Source invalid: Length must be known");

    // With CCM the length is restricted, let's check that here
    if(GetMaxPayloadLength(GetNonceSize()) < len)
        throw Exception<>("Payload too large for the given nonce size");

    if(NULL == nonce){
        // Initialize a random IV
        d->rng.GenerateBlock(n.Data(), GetNonceSize());
    }
    d->enc.SetKeyWithIV(m_key, sizeof(m_key), nonce == NULL ? n.Data():nonce, GetNonceSize());
    d->enc.SpecifyDataLengths(aData_len + m_authData.Length(), len);

    GUINT32 read = 0;
    GUINT32 to_read = chunk_size == 0 ? len : chunk_size;
    GUINT32 buf_sz = to_read;
    ::CryptoPP::AuthenticatedEncryptionFilter ef(
                d->enc,
                new OutputInterfaceSink(*out),
                false,
                TagLength);

    // First pass the authenticated data. Our second key goes in here.
    ef.ChannelPut(::CryptoPP::AAD_CHANNEL, m_authData, m_authData.Length());

    // If they gave us additional auth data, add it here
    if(aData && 0 < aData->BytesAvailable())
    {
        SmartArrayPointer<byte> a(new byte[aData_len]);
        if(aData_len != aData->ReadBytes(a, aData_len, aData_len))
            throw Exception<>("Error reading from auth data source");
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
                throw Exception<>("Error reading from source");

            ef.Put(buf, to_read, true);
            if(out)
                out->Flush();
            read += to_read;

            if(progress_cb(((float)read / len) * 100))
                throw CancelledOperationException<>();
        }
    }
    ef.MessageEnd();

    // Write the IV at the very end
    out->WriteBytes(nonce == NULL ? n.Data():nonce, GetNonceSize());
    out->Flush();
}


void Cryptor::DecryptData(IOutput *out,
                          IRandomAccessInput *cData,
                          IInput *aData,
                          GUINT32 chunk_size,
                          function<bool(int)> progress_cb)
{
    G_D;
    GUINT64 ct_len, len;
    progress_cb(0);
    finally([=]{ progress_cb(100); });
    const GUINT64 aData_len = aData ? aData->BytesAvailable() : 0;
    if(cData == NULL || (ct_len = cData->BytesAvailable()) < GetCrypttextSizeDiff())
        throw Exception<>("Invalid data length");
    len = ct_len - GetCrypttextSizeDiff();

    if(GetMaxPayloadLength(GetNonceSize()) < len)
        throw Exception<>("Payload too large for the given nonce size");

    // Read the MAC tag and IV at the end of the crypttext
    SmartArrayPointer<byte> mac_iv(new byte[GetCrypttextSizeDiff()]);
    cData->Seek(cData->Length() - GetCrypttextSizeDiff());
    if(GetCrypttextSizeDiff() !=
            cData->ReadBytes(mac_iv.Data(), GetCrypttextSizeDiff(), GetCrypttextSizeDiff()))
        throw Exception<>("Error reading from source");

    // Seek back to the start of the message
    cData->Seek(0);

    // Initialize the decryptor
    d->dec.SetKeyWithIV(m_key, sizeof(m_key), mac_iv.Data() + TagLength, GetNonceSize());
    d->dec.SpecifyDataLengths(aData_len + m_authData.Length(), len);
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
        df.ChannelPut(::CryptoPP::AAD_CHANNEL, m_authData, m_authData.Length());

        // If they gave us additional auth data, add it here
        if(aData && 0 < aData->BytesAvailable())
        {
            SmartArrayPointer<byte> a(new byte[aData_len]);
            if(aData_len != aData->ReadBytes(a, aData_len, aData_len))
                throw Exception<>("Error reading from auth data source");
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
                    throw Exception<>("Error reading from source");

                df.Put(buf, to_read, true);
                if(out)
                    out->Flush();
                read += to_read;

                if(progress_cb(((float)read / len) * 100))
                    throw CancelledOperationException<>();
            }
        }

        // This will throw an exception if authenticity validation failed
        df.MessageEnd();
    }
    catch(const ::CryptoPP::Exception &ex)
    {
        throw AuthenticationException<>(ex.what());
    }

    // Seek the input device to the end of the message
    cData->Seek(ct_len);

    if(out) out->Flush();
}

double Cryptor::GetMaxKeyUsageSuggestion(GUINT8 nonce_length)
{
    double ret = 0;
    if(7 <= nonce_length && nonce_length <= 13){
        ret = pow(2, 8*nonce_length - 32);
    }
    return ret;
}



void Cryptor::DefaultKeyDerivation::DeriveKeyFromPassword(byte *key, const char *password)
{
    ::CryptoPP::SHA3_256 h;
    const GUINT32 pw_len = password ? strlen(password) : 0;

    // Hash it once with the salt
    h.Update(m_salt.ConstData(), m_salt.Length());
    h.Update((byte const *)password, pw_len);
    h.Final(key);

    // Then hash the result again a bunch more times
    for(GUINT32 i = 0; i < KDF_ITERATIONS; ++i){
        h.Update(key, Cryptor::KeySize);
        h.Final(key);
    }
}

void Cryptor::DefaultKeyDerivation::DeriveKeyFromKeyfile(byte *key, const char *keyfile)
{
    SmartPointer<IInput> in;
    if(keyfile != NULL && strlen(keyfile) > 0){
        File *f = new File(keyfile);
        in = f;
        f->Open(File::OpenRead);
    }

    // Since the keyfile has enough entropy, we only hash this once (the attacker will not
    //  do a precomputed dictionary attack when the input is random data.  They may as well
    //  guess randomly at the key).
    Hash<SHA3_256> h;
    h.AddData(m_salt.ConstData(), m_salt.Length());
    h.AddDataFromDevice(in);
    h.Final(key);
}

Vector<byte> Cryptor::DefaultKeyDerivation::DeriveAuthData(const char *password)
{
    ::CryptoPP::SHA3_512 h;
    byte result[h.DIGESTSIZE];
    const GUINT32 pw_len = password ? strlen(password) : 0;

    // Hash it once with the salt
    h.Update(m_salt.ConstData(), m_salt.Length());
    h.Update((byte const *)password, pw_len);
    h.Final(result);

    // Then hash it again a bunch more times using each successive hash
    //  as a salt for the next iteration.
    for(GUINT32 i = 0; i < KDF_ITERATIONS; ++i){
        h.Update(result, sizeof(result));
        h.Update((byte const *)password, pw_len);
        h.Final(result);
    }
    return Vector<byte>(result, sizeof(result));
}

IClonable *Cryptor::DefaultKeyDerivation::Clone() const noexcept
{
    return new DefaultKeyDerivation(*this);
}


END_NAMESPACE_GUTIL1;
