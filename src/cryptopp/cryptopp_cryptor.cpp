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
#include <cryptopp/modes.h>
#include <cryptopp/aes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>
#include <cryptopp/files.h>
using namespace CryptoPP;

// These are some constants for the type of encryption we've chosen
#define KEYLENGTH   32
#define BLOCKSIZE   16
#define IVLENGTH    BLOCKSIZE

namespace
{

// These are allocated at compile time, so consumers
//   of our class don't need to include cryptopp headers
struct d_t
{
    AutoSeededX917RNG<AES> rng;
    OFB_Mode<AES>::Encryption enc;
    OFB_Mode<AES>::Decryption dec;
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
    G_D;
    __compute_password_hash(d->key, password);
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


void Cryptor::EncryptData(byte const *plaintext, GUINT32 len, OutputInterface *output)
{
    G_D;

    // Initialize a random IV and initialize the encryptor
    byte iv[IVLENGTH];
    d->rng.GenerateBlock(iv, IVLENGTH);
    d->enc.SetKeyWithIV(d->key, KEYLENGTH, iv);

    {
        StreamTransformationFilter stf(d->enc, new OutputInterfaceSink(*output), StreamTransformationFilter::NO_PADDING);
        stf.Put(plaintext, len);
        stf.MessageEnd();
    }

    // Append the IV
    output->WriteBytes(iv, IVLENGTH);
    output->Flush();
}


void Cryptor::DecryptData(byte const *crypttext, GUINT32 len, OutputInterface *output)
{
    if(len < IVLENGTH)
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Invalid data length");

    G_D;

    // Initialize the decryptor
    d->dec.SetKeyWithIV(d->key, KEYLENGTH, crypttext + len - IVLENGTH);

    StreamTransformationFilter stf(d->dec, new OutputInterfaceSink(*output), StreamTransformationFilter::NO_PADDING);
    stf.Put(crypttext, len - IVLENGTH);
    stf.MessageEnd();
    output->Flush();
}

void Cryptor::EncryptFile(const char *filename, OutputInterface *output)
{
    G_D;

    if(!File::Exists(filename))
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "File not found");

    // Initialize a random IV and initialize the encryptor
    byte iv[IVLENGTH];
    d->rng.GenerateBlock(iv, IVLENGTH);
    d->enc.SetKeyWithIV(d->key, KEYLENGTH, iv);

    FileSource(filename, true,
               new StreamTransformationFilter(
                   d->enc, new OutputInterfaceSink(*output),
                   StreamTransformationFilter::NO_PADDING)
               );

    // Append the IV
    output->WriteBytes(iv, IVLENGTH);
    output->Flush();
}

void Cryptor::DecryptFile(const char *filename, OutputInterface *output)
{
    G_D;
    File f(filename);
    GUINT32 len;
    if(!f.Exists())
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "File not found");
    f.Open(File::OpenRead);
    if((len = f.Length()) < IVLENGTH)
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Invalid data length");

    // Read the IV
    byte iv[IVLENGTH];
    f.Seek(len - IVLENGTH);
    if(IVLENGTH != f.Read(iv, IVLENGTH, IVLENGTH))
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Unable to read from file");

    // Initialize the decryptor
    d->dec.SetKeyWithIV(d->key, KEYLENGTH, iv);

    lword ct_len = len - IVLENGTH;
    FileSource(filename, false,
               new StreamTransformationFilter(
                   d->dec,
                   new OutputInterfaceSink(*output),
                   StreamTransformationFilter::NO_PADDING)
               ).Pump2(ct_len);
    output->Flush();
}


END_NAMESPACE_GUTIL;
