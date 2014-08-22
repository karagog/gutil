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
#include "gutil_iointerface.h"
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
#define IVLENGTH    16

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


static void __compute_password_hash(byte result[KEYLENGTH], const char *password)
{
    SHA256().CalculateDigest(result, (byte const *)password, strlen(password));
}


Cryptor::Cryptor(const char *password)
{
    G_D_INIT();
    ChangePassword(password);
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

bool Cryptor::CheckPassword(const char *password) const
{
    G_D;
    byte buf[KEYLENGTH];
    __compute_password_hash(buf, password);
    return 0 == memcmp(d->key, buf, KEYLENGTH);
}

void Cryptor::ChangePassword(const char *password)
{
    G_D;
    __compute_password_hash(d->key, password);
}


void Cryptor::EncryptData(byte const *plaintext, GUINT32 len, OutputInterface *output) const
{
    G_D;

    // Initialize a random IV and initialize the encryptor
    byte iv[IVLENGTH];
    d->rng.GenerateBlock(iv, IVLENGTH);
    d->enc.SetKeyWithIV(d->key, KEYLENGTH, iv, IVLENGTH);

    AuthenticatedEncryptionFilter ef(d->enc, new OutputInterfaceSink(*output));
    ef.Put(plaintext, len);
    ef.MessageEnd();

    // Append the IV
    output->WriteBytes(iv, IVLENGTH);
    output->Flush();
    delete output;
}


void Cryptor::DecryptData(byte const *crypttext, GUINT32 len, OutputInterface *output) const
{
    G_D;
    SmartPointer<OutputInterface> out(output);
    if(len < IVLENGTH)
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Invalid data length");
    len = len - IVLENGTH;

    // Initialize the decryptor
    d->dec.SetKeyWithIV(d->key, KEYLENGTH, crypttext + len, IVLENGTH);
    try
    {
        AuthenticatedDecryptionFilter df(d->dec,
                                         output == NULL ? NULL : new OutputInterfaceSink(*output),
                                         AuthenticatedDecryptionFilter::THROW_EXCEPTION);
        df.Put(crypttext, len);
        df.MessageEnd();
    }
    catch(const CryptoPP::Exception &ex)
    {
        THROW_NEW_GUTIL_EXCEPTION2(AuthenticationException, ex.what());
    }
    if(output) output->Flush();
}

void Cryptor::EncryptFile(const char *filename, OutputInterface *output, GUINT32 chunk_size, IProgressHandler *ph) const
{
    G_D;
    SmartPointer<OutputInterface> out(output);
    byte iv[IVLENGTH];
    if(ph) ph->ProgressUpdated(0);

    File f(filename);
    if(!f.Exists())
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "File not found");
    f.Open(File::OpenRead);

    // Initialize a random IV and initialize the encryptor
    d->rng.GenerateBlock(iv, IVLENGTH);
    d->enc.SetKeyWithIV(d->key, KEYLENGTH, iv, IVLENGTH);

    GUINT32 len = f.Length();
    GUINT32 read = 0;
    GUINT32 to_read = chunk_size == 0 ? len : chunk_size;
    GUINT32 buf_sz = to_read;
    SmartArrayPointer<byte> buf( new byte[buf_sz] );
    AuthenticatedEncryptionFilter ef(d->enc, new OutputInterfaceSink(*output));
    while(read < len)
    {
        if((read + to_read) > len)
            to_read = len - read;

        if(to_read != f.Read(buf, buf_sz, to_read))
            THROW_NEW_GUTIL_EXCEPTION2(Exception, "Problem reading file");

        ef.Put(buf, to_read, true);
        read += to_read;

        if(ph){
            ph->ProgressUpdated(((float)read / len) * 100);
            if(ph->CancelOperation())
                THROW_NEW_GUTIL_EXCEPTION(CancelledOperationException);
        }
    }
    ef.MessageEnd();

    // Append the IV
    output->WriteBytes(iv, IVLENGTH);
    output->Flush();
}

void Cryptor::DecryptFile(const char *filename, OutputInterface *output, GUINT32 chunk_size, IProgressHandler *ph) const
{
    G_D;
    SmartPointer<OutputInterface> out(output);
    File f(filename);
    GUINT32 len;
    if(!f.Exists())
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "File not found");
    f.Open(File::OpenRead);
    if((len = f.Length()) < IVLENGTH)
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Invalid data length");
    len = len - IVLENGTH;

    // Read the IV from the end of the file
    byte iv[IVLENGTH];
    f.Seek(len);
    if(IVLENGTH != f.Read(iv, IVLENGTH, IVLENGTH))
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Unable to read from file");
    f.Seek(0);

    // Initialize the decryptor
    d->dec.SetKeyWithIV(d->key, KEYLENGTH, iv, IVLENGTH);
    try
    {
        GUINT32 read = 0;
        GUINT32 to_read = chunk_size == 0 ? len : chunk_size;
        GUINT32 buf_sz = to_read;
        SmartArrayPointer<byte> buf( new byte[buf_sz] );
        AuthenticatedDecryptionFilter df(d->dec,
                       output == NULL ? NULL : new OutputInterfaceSink(*output),
                       AuthenticatedDecryptionFilter::THROW_EXCEPTION);
        while(read < len)
        {
            if((read + to_read) > len)
                to_read = len - read;

            if(to_read != f.Read(buf, buf_sz, to_read))
                THROW_NEW_GUTIL_EXCEPTION2(Exception, "Problem reading file");

            df.Put(buf, to_read, true);
            read += to_read;

            if(ph){
                ph->ProgressUpdated(((float)read / len) * 100);
                if(ph->CancelOperation())
                    THROW_NEW_GUTIL_EXCEPTION(CancelledOperationException);
            }
        }
        df.MessageEnd();
    }
    catch(const CryptoPP::Exception &ex)
    {
        THROW_NEW_GUTIL_EXCEPTION2(AuthenticationException, ex.what());
    }
    if(output) output->Flush();
}


END_NAMESPACE_GUTIL;
