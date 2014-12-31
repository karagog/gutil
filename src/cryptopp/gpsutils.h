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

#ifndef GUTIL_GPSUTILS_H
#define GUTIL_GPSUTILS_H

#include <gutil/file.h>
#include <gutil/smartpointer.h>
#include <gutil/cryptopp_hash.h>
#include <gutil/cryptopp_cryptor.h>

/** \file
 *  Classes for managing Portable Safe files (GPS).
 *
 *  The GPS file format is a secure way to store data and transfer
 *  it between applications.
 *
 *  The format is as follows. The first byte is the plaintext version byte that tells
 *  us what version the file format is. The next 25 bytes of the file make up the encrypted
 *  file header. The GPS file is designed to be completely opaque, so without successfully
 *  decrypting this file header, the rest of the file is
 *  meaningless and its data completely unintelligible. You can't tell how many payloads
 *  there are, or how big they are.
 *
 *  After the file header come any amount of payloads, one after the other. There
 *  can be 0 to infinity of them, and each one has its own header (the payload header)
 *  to tell us the size of the payload, and also contain application-specific user
 *  data pertaining to the payload. The size of this "user data" section is determined by the
 *  information in the file header. It can be anywhere from 0 to 2^16-1.
*/

NAMESPACE_GUTIL1(CryptoPP);
class Cryptor;
END_NAMESPACE_GUTIL1;

#define GPS_HASH_DIGEST_LENGTH  28

NAMESPACE_GUTIL;


/** A class for exporting to GPS. */
class GPSFile_Export
{
    GUTIL_DISABLE_COPY(GPSFile_Export);
    GUtil::File m_file;
    GUtil::SmartPointer<GUtil::CryptoPP::Cryptor> m_cryptor;
    GUtil::SmartPointer<GUtil::CryptoPP::Hash<>> m_hash;
    const GUINT16 m_userDataSize;
public:

    /** Over time the format of GPS files may change. This tracks
     *  the version that this code is on.
    */
    const static int VERSION = 0;

    /** Constructs an export object.
     *  \param filepath The file to export to
     *  \param userdata_size The amount of user data (in bytes) you want to store in
     *              each header.
    */
    GPSFile_Export(const char *filepath,
                   const GUtil::CryptoPP::Cryptor::Credentials &,
                   GUINT16 userdata_size = 0);
    ~GPSFile_Export();

    /** Returns the size of the user data field of each payload header, in bytes. */
    GUINT16 UserDataSize() const{ return m_userDataSize; }

    /** Appends the header and payload to the GPS file.
     *
     *  \param user_data Used to populate the user data of the header.
     *          If non-null, it can be max UserDataSize() bytes long.
     *          Any bytes you don't write in the user data section will be 0.
    */
    void AppendPayload(byte const *payload, GUINT32 payload_length,
                       byte const *user_data = NULL,
                       GUINT16 user_data_len = 0);

    void AppendPayloadFile(const char *payload_file,
                           byte const *user_data = NULL,
                           GUINT16 user_data_len = 0,
                           GUINT32 chunk_size = 0,
                           IProgressHandler *ph = 0);

private:

    void _write_payload_header(GUINT64, byte const *, GUINT16);

};


/** A class for importing from GPS */
class GPSFile_Import
{
    GUTIL_DISABLE_COPY(GPSFile_Import);
    GUtil::File m_file;
    GUtil::SmartPointer<GUtil::CryptoPP::Cryptor> m_cryptor;
    GUtil::SmartPointer<GUtil::CryptoPP::Hash<>> m_hash;

    bool m_payloadRead;
    GUINT64 m_payloadLength;
    GUtil::SmartArrayPointer<byte> m_userData;
    GUINT16 m_userDataSize;
    bool m_validateChecksum;
    byte m_finalHash[GPS_HASH_DIGEST_LENGTH];
public:

    /** Over time the format of GPS files may change. This tracks
     *  the version that this code is on.
    */
    const static int VERSION = GPSFile_Export::VERSION;

    /** Constructs an import object.
     *  \param filepath The path to the GPS you want to import from.
     *  \param validate_checksum If this is true, then the entire file's contents
     *  will be validated for authenticity. The last time you call NextPayload()
     *  when it returns false at the end of the file, if the file contents do not
     *  match the checksum then it will throw an AuthenticationException. There is
     *  a small overhead to doing this, but will guarantee the file's content authenticity.
    */
    GPSFile_Import(const char *filepath,
                   const GUtil::CryptoPP::Cryptor::Credentials &,
                   bool validate_checksum = true);
    ~GPSFile_Import();

    /** Returns the size of the user data field of each payload header, in bytes. */
    GUINT16 UserDataSize() const{ return m_userDataSize; }

    /** Navigates to the next payload in the file.
     *  \returns True if it successfully reached the next payload, false if there
     *  are no more.
    */
    bool NextPayload();

    /** Returns the user data array of the current payload. The size of the array is
     *  given by UserDataSize(). This data is only valid if NextPayload returned true.
    */
    byte const *CurrentUserData(){ return m_userData.Data(); }

    /** Returns the length of the current payload in bytes. */
    GUINT32 CurrentPayloadSize() const{ return m_payloadLength; }

    /** Writes the current payload to the destination buffer. The buffer must
     *  be at least CurrentPayloadSize() large.
    */
    void GetCurrentPayload(byte *dest, GUINT32 chunk_size = 0, GUtil::IProgressHandler * = NULL);

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_GPSUTILS_H
