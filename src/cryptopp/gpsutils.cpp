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

#include "gpsutils.h"
#include "cryptopp_rng.h"
#include "cryptopp_sinks.h"
#include <gutil/sourcesandsinks.h>
#include <gutil/smartpointer.h>
USING_NAMESPACE_GUTIL1(CryptoPP);

#define SALT_LENGTH         8
#define GPS_HEADER_LENGTH   2 + GPS_HASH_DIGEST_LENGTH

// Pick the smallest size nonce, because if they don't know the key then
// the only nonces they'll be able to identify are on the header and the one
// at the end of the file, therefore we don't need to worry about duplicate nonces
// as long as they're random, so we can effectively reuse the same key here infinitely
// many times and also maximize theoretical payload lengths. I really can have my cake
// and eat it too!
#define NONCE_LENGTH 7

NAMESPACE_GUTIL;


GPSFile_Export::GPSFile_Export(const char *filepath,
                               const Cryptor::Credentials &creds,
                               GUINT16 userdata_size)
    :m_file(filepath),
      m_hash(new Hash<SHA3_224>),
      m_userDataSize(userdata_size)
{
    // Prepare the version + salt, to be written first in plaintext
    byte version_salt[1 + SALT_LENGTH];
    byte *salt = version_salt + 1;

    version_salt[0] = VERSION;
    CryptoPP::RNG().Fill(salt, SALT_LENGTH);

    // Write the version + salt in plaintext
    m_file.Open(File::OpenReadWriteTruncate);
    m_file.Write(version_salt, sizeof(version_salt));

    // Write a placeholder for the encrypted header, which we'll write in the destructor
    byte placeholder[GPS_HEADER_LENGTH + Cryptor::TagLength + NONCE_LENGTH] = {};
    m_file.Write(placeholder, sizeof(placeholder));

    // Initialize the cryptor
    m_cryptor = new Cryptor(creds, NONCE_LENGTH, new Cryptor::DefaultKeyDerivation(salt, SALT_LENGTH));
}

GPSFile_Export::~GPSFile_Export()
{
    // Write the file header
    m_file.Seek(1 + SALT_LENGTH);

    // Prepare the plaintext
    byte header_pt[GPS_HEADER_LENGTH];
    header_pt[0] = (UserDataSize() >> 8) & 0x0FF;
    header_pt[1] = UserDataSize() & 0x0FF;

    // Get the final hash
    m_hash->Final(header_pt + 2);

    // Write the encrypted header
    ByteArrayInput i(header_pt, sizeof(header_pt));
    m_cryptor->EncryptData(&m_file, &i);
}

void GPSFile_Export::AppendPayload(byte const *payload, GUINT32 payload_length,
                                   byte const *user_data, GUINT16 user_data_len)
{
    if(user_data_len > UserDataSize())
        throw Exception<>("Payload metadata too long");

    // The first thing we write is the payload header
    _write_payload_header(payload_length, user_data, user_data_len);

    // Add the payload to the hash
    m_hash->AddData(payload, payload_length);

    // Now encrypt and write the payload
    ByteArrayInput i(payload, payload_length);
    m_cryptor->EncryptData(&m_file, &i);
}

void GPSFile_Export::AppendPayloadFile(const char *payload_file,
                                       byte const *user_data,
                                       GUINT16 user_data_len,
                                       GUINT32 chunk_size,
                                       IProgressHandler *ph)
{
    if(user_data_len > UserDataSize())
        throw Exception<>("Payload metadata too long");

    File f(payload_file);
    f.Open(File::OpenRead);

    // The first thing we write is the payload header
    _write_payload_header(f.Length(), user_data, user_data_len);

    // Add the payload to the hash
    m_hash->AddDataFromDevice(&f, chunk_size, ph);

    // Now encrypt and write the payload
    f.Seek(0);
    m_cryptor->EncryptData(&m_file, &f, NULL, NULL, chunk_size, ph);
}

#define PAYLOAD_LENGTH_BYTES    8

void GPSFile_Export::_write_payload_header(GUINT64 payload_length,
                                           byte const *user_data,
                                           GUINT16 user_data_len)
{
    const GUINT32 header_len = PAYLOAD_LENGTH_BYTES + UserDataSize();
    SmartArrayPointer<byte> header_pt(new byte[header_len]);

    // Set the length bytes
    for(int i = 0; i < PAYLOAD_LENGTH_BYTES; ++i)
        header_pt[i] = (payload_length >> (64 - 8*(i + 1))) & 0x0FF;

    // The rest of the bytes are filled with user data
    if(user_data && 0 < user_data_len)
        memcpy(header_pt.Data() + PAYLOAD_LENGTH_BYTES, user_data, user_data_len);

    // Any bytes not initialized are set to 0
    if(user_data_len < UserDataSize())
        memset(header_pt.Data() + PAYLOAD_LENGTH_BYTES + user_data_len, 0,
               header_len - PAYLOAD_LENGTH_BYTES - user_data_len);

    m_hash->AddData(header_pt, header_len);

    // Encrypt and write header
    ByteArrayInput i(header_pt, header_len);
    m_cryptor->EncryptData(&m_file, &i);
}




GPSFile_Import::GPSFile_Import(const char *filepath,
                               const Cryptor::Credentials &creds,
                               bool validate)
    :m_file(filepath),
      m_hash(validate ? new Hash<SHA3_224> : NULL),
      m_payloadRead(true),
      m_payloadLength(0),
      m_validateChecksum(validate)
{
    byte version_salt[9];
    byte *salt = version_salt + 1;
    byte header_pt[GPS_HEADER_LENGTH];
    m_file.Open(File::OpenRead);
    if(m_file.Length() < (sizeof(version_salt) + sizeof(header_pt) + m_cryptor->TagLength + NONCE_LENGTH))
        throw Exception<>("Invalid file (too small)");

    // Read in the version + salt
    if(sizeof(version_salt) != m_file.Read(version_salt, sizeof(version_salt), sizeof(version_salt)))
        throw Exception<>("Error reading from file");

    if(version_salt[0] > VERSION)
        throw Exception<>("This file's version is too advanced for me");

    // Initialize the cryptor
    m_cryptor = new Cryptor(creds, NONCE_LENGTH, new Cryptor::DefaultKeyDerivation(salt, SALT_LENGTH));

    // Read in and decrypt the header:
    {
        ConstrainedInput i(m_file,
                           m_file.Pos(),
                           m_file.Pos() + sizeof(header_pt) + m_cryptor->TagLength + NONCE_LENGTH);
        ByteArrayOutput o(header_pt);
        m_cryptor->DecryptData(&o, &i);
    }

    // Now we know the size of the user data
    m_userDataSize = ((GUINT16)header_pt[0] << 8) | header_pt[1];
    m_userData.Set(new byte[UserDataSize()]);

    // Save the hash for later comparison
    if(m_validateChecksum)
        memcpy(m_finalHash, header_pt + 2, sizeof(m_finalHash));
}

GPSFile_Import::~GPSFile_Import() {}

bool GPSFile_Import::NextPayload()
{
    const GUINT32 header_len = PAYLOAD_LENGTH_BYTES + UserDataSize();
    const GUINT32 header_len_ct = header_len + m_cryptor->TagLength + NONCE_LENGTH;

    // If they didn't read the current payload, then read it in now and add it to
    // the hash, or skip over it
    if(!m_payloadRead){
        if(m_validateChecksum)
            GetCurrentPayload(NULL);
        else
            m_file.Seek(m_file.Pos() + CurrentPayloadSize() + m_cryptor->TagLength + NONCE_LENGTH);
        m_payloadRead = true;
    }

    if(m_file.BytesAvailable() < header_len_ct){
        // We hit the end of the file, so compare the hash with what we read in the header
        if(m_validateChecksum){
            byte calculated[GPS_HASH_DIGEST_LENGTH];
            m_hash->Final(calculated);
            if(0 != memcmp(calculated, m_finalHash, GPS_HASH_DIGEST_LENGTH))
                throw AuthenticationException<>("Checksum does not match received data");
        }
        return false;
    }

    // Read in and decrypt the payload header
    SmartArrayPointer<byte> header_pt(new byte[header_len]);
    {
        ConstrainedInput i(m_file, m_file.Pos(), m_file.Pos() + header_len_ct);
        ByteArrayOutput bao(header_pt);
        OutputReplicator o(1, &bao);
        SmartPointer<IOutput> ho;
        if(m_validateChecksum)
            o.AddOutputDevice(ho = new HashOutput(*m_hash));
        m_cryptor->DecryptData(&o, &i);
    }

    // First interpret the payload length:
    m_payloadLength = 0;
    for(int i = 0; i < PAYLOAD_LENGTH_BYTES; ++i)
        m_payloadLength |= header_pt[i] << (64 - 8*(i+1));

    // Copy the userdata into our buffer
    memcpy(m_userData.Data(), header_pt.Data() + PAYLOAD_LENGTH_BYTES, UserDataSize());
    m_payloadRead = false;
    return true;
}

void GPSFile_Import::GetCurrentPayload(byte *dest, GUINT32 chunk_size, IProgressHandler *ph)
{
    if(m_payloadRead)
        throw Exception<>("Payload already gotten");

    OutputReplicator o;
    SmartPointer<IOutput> o1;
    SmartPointer<IOutput> o2;
    if(dest)
        o.AddOutputDevice(o1 = new ByteArrayOutput(dest));
    if(m_validateChecksum)
        o.AddOutputDevice(o2 = new HashOutput(*m_hash));

    ConstrainedInput i(m_file, m_file.Pos(),
                       m_file.Pos() + CurrentPayloadSize() + m_cryptor->TagLength + NONCE_LENGTH);
    if(o.NumberOfOutputs() > 0)
        m_cryptor->DecryptData(&o, &i, NULL, chunk_size, ph);
    else
        i.Seek(i.Length());
    m_payloadRead = true;
}


END_NAMESPACE_GUTIL;
