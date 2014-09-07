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

#include "gpsutils.h"
#include "cryptopp_cryptor.h"
#include "cryptopp_rng.h"
#include "gutil_sourcesandsinks.h"
#include "gutil_smartpointer.h"
USING_NAMESPACE_GUTIL1(CryptoPP);

// Pick the smallest size nonce, because if they don't know the key then
// the only nonces they'll be able to identify are on the header and the one
// at the end of the file, therefore we don't need to worry about duplicate nonces
// as long as they're random, so we can effectively reuse the same key here infinitely
// many times and also maximize theoretical payload lengths. I really can have my cake
// and eat it too!
#define NONCE_LENGTH 7

NAMESPACE_GUTIL;


GPSFile_Export::GPSFile_Export(const char *filepath,
                               const char *password,
                               const char *keyfile,
                               GUINT16 userdata_size)
    :m_file(filepath),
      m_userDataSize(userdata_size)
{
    // Prepare the version + salt, to be written first in plaintext
    byte version_salt[9];
    byte *salt = version_salt + 1;
    const GUINT32 salt_len = sizeof(version_salt) - 1;

    version_salt[0] = VERSION;
    CryptoPP::RNG().Fill(salt, salt_len);

    // Prepare the userdata size bytes, to be encrypted in the header
    byte header_pt[2];
    header_pt[0] = (userdata_size >> 8) & 0x0FF;
    header_pt[1] = userdata_size & 0x0FF;

    // Initialize the cryptor
    m_cryptor = new Cryptor(password, keyfile, salt, salt_len);
    m_cryptor->SetNonceSize(NONCE_LENGTH);

    // Write the version + salt in plaintext
    m_file.Open(File::OpenReadWriteTruncate);
    m_file.Write(version_salt, sizeof(version_salt));

    // Write the encrypted header
    ByteArrayInput i(header_pt, sizeof(header_pt));
    m_cryptor->EncryptData(&m_file, &i);
}

GPSFile_Export::~GPSFile_Export() {}

void GPSFile_Export::AppendPayload(byte const *payload, GUINT32 payload_length,
                                   byte const *user_data, GUINT16 user_data_len)
{
    if(user_data_len > UserDataSize())
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Payload metadata too long");

    // The first thing we write is the payload header
    _write_payload_header(payload_length, user_data, user_data_len);

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
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Payload metadata too long");

    File f(payload_file);
    f.Open(File::OpenRead);

    // The first thing we write is the payload header
    _write_payload_header(f.Length(), user_data, user_data_len);

    // Now encrypt and write the payload
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

    // Encrypt and write header
    ByteArrayInput i(header_pt, header_len);
    m_cryptor->EncryptData(&m_file, &i);
}




GPSFile_Import::GPSFile_Import(const char *filepath,
                               const char *password,
                               const char *keyfile)
    :m_file(filepath),
      m_payloadRead(true),
      m_payloadLength(0)
{
    byte version_salt[9];
    byte *salt = version_salt + 1;
    const GUINT32 salt_len = sizeof(version_salt) - 1;
    byte header_pt[2];
    m_file.Open(File::OpenRead);
    if(m_file.Length() < (sizeof(version_salt) + sizeof(header_pt) + m_cryptor->TagLength + NONCE_LENGTH))
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Invalid file (too small)");

    // Read in the version + salt
    if(sizeof(version_salt) != m_file.Read(version_salt, sizeof(version_salt), sizeof(version_salt)))
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Error reading from file");

    if(version_salt[0] > VERSION)
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "This file's version is too advanced for me");

    // Initialize the cryptor
    m_cryptor = new Cryptor(password, keyfile, salt, salt_len);
    m_cryptor->SetNonceSize(NONCE_LENGTH);

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
}

GPSFile_Import::~GPSFile_Import() {}

bool GPSFile_Import::NextPayload()
{
    const GUINT32 header_len = PAYLOAD_LENGTH_BYTES + UserDataSize();
    const GUINT32 header_len_ct = header_len + m_cryptor->TagLength + NONCE_LENGTH;

    if(m_file.BytesAvailable() < header_len_ct)
        return false;

    // If they didn't read the current payload, then skip over it
    if(!m_payloadRead){
        m_file.Seek(m_file.Pos() + CurrentPayloadSize() + m_cryptor->TagLength + NONCE_LENGTH);
        m_payloadRead = true;
    }

    // Read in and decrypt the payload header
    SmartArrayPointer<byte> header_pt(new byte[header_len]);
    {
        ConstrainedInput i(m_file, m_file.Pos(), m_file.Pos() + header_len_ct);
        ByteArrayOutput o(header_pt.Data());
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
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Payload already gotten");

    ConstrainedInput i(m_file, m_file.Pos(),
                       m_file.Pos() + CurrentPayloadSize() + m_cryptor->TagLength + NONCE_LENGTH);
    ByteArrayOutput o(dest);
    m_cryptor->DecryptData(&o, &i, NULL, chunk_size, ph);
    m_payloadRead = true;
}


END_NAMESPACE_GUTIL;
