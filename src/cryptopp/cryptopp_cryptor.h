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

#ifndef GUTIL_CRYPTOPP_CRYPTOR_H
#define GUTIL_CRYPTOPP_CRYPTOR_H

#include "gutil_iointerface.h"

NAMESPACE_GUTIL;


/** Use this class to encrypt and decrypt data securely and with message authentication.
 *
 *  Authentication means that on decrypting it will recognize if the message has been
 *  tampered with or corrupted. This assurance comes at a small cost of 16 additional bytes
 *  added onto every encrypted message.
 *
 *  The crypttext output of all functions will contain a 16-byte
 *  Message Authentication Code (MAC) and the 12-byte Initialization Vector that are
 *  required to decrypt the message. The total size of the crypttext will be the size of
 *  the plaintext data plus the MAC and IV, in other words 28 bytes plus pDataLen.
 *
 *  You can optionally supply authenticated data to the encryption functions, which will
 *  not add to the size of the crypttext (you will transmit this data in clear text),
 *  but it will nonetheless be required to decrypt the message.
*/
class Cryptor
{
    void *d;
public:

    /** Constructs and initializes the Cryptor.
     *
     *  \param password The password to be used for encryption/decryption, must be in UTF-8 format.
     *      After initialization, the Cryptor has no idea what the password is anymore, but you can check if
     *      another one matches by using the CheckPassword function.
    */
    Cryptor(const char *password);

    /** Duplicates the cryptor, taking on the other's password. */
    Cryptor(const Cryptor &);
    ~Cryptor();

    /** Returns true if the password is correct, by comparing hashes. */
    bool CheckPassword(const char *) const;

    /** Changes the password used by the cryptor. */
    void ChangePassword(const char *password);


    /** An interface that allows you to handle progress updates, and also cancel the operation. */
    class IProgressHandler
    {
    public:
        /** Called periodically with an updated progress value between 0 (started) and 100 (finished)*/
        virtual void ProgressUpdated(int) = 0;

        /** Called periodically to ask if the operation should be cancelled. Return true if you want to cancel. */
        virtual bool CancelOperation(){ return false; }
    };


    /** Encrypts the string with authentication.
     *
     *  \param pData The plaintext data that will be encrypted AND authenticated. This may be
     *          null, but then you must provide authenticated data (one or the other, or both, but
     *          not neither, according to the NIST document on GCM)
     *  \param out The crypttext will be pushed here.
     *  \param aData The authenticated data will be authenticated but NOT encrypted. This is
     *          optional, but if given this data will be required to correctly decrypt the message.
     *          It will not contribute to the size of the crypttext output but will be
     *          required to decrypt the message. You will transmit this in clear text
     *          along with the crypttext.
     *  \param chunk_size Optionally process the file in chunks (given in bytes).
     *          If you pass 0 it does the whole file in one go.
     *  \param ph An optional progress handler for long operations; large data sources.
     *          In order for this to be useful, you should set a reasonable value for chunk_size.
    */
    void EncryptData(InputInterface *pData,
                     OutputInterface *out,
                     InputInterface *aData = NULL,
                     GUINT32 chunk_size = 0,
                     IProgressHandler *ph = 0) const;

    /** Decrypts the string and throw an exception if decryption failed.
     *  It could fail due to a wrong key, wrong data (length), or if a bit was flipped somewhere
     *  either maliciously or accidentally, or the initialization vector wasn't right
     *
     *  \param cData The crypttext data that was produced by EncryptData.
     *  \param out The recovered plaintext will be pushed here.
     *              This paramater may be null, in which case you won't get the plaintext, but
     *              you can still validate the authenticity of the message.
     *  \param aData The authenticated data that is associated with the crypttext. This is
     *              only used if you encrypted data with the aData parameter.
     *  \param chunk_size Optionally process the file in chunks (given in bytes).
     *          If you pass 0 it does the whole file in one go.
     *  \param ph An optional progress handler for long operations; large data sources.
     *          In order for this to be useful, you should set a reasonable value for chunk_size.
     *  \throws A GUtil exception if decryption failed. It could fail due to a wrong key,
     *              wrong data (or length), or if a bit was flipped somewhere,
     *              or the initialization vector wasn't right (or wasn't there).
    */
    void DecryptData(InputInterface *cData,
                     OutputInterface *out,
                     InputInterface *aData = NULL,
                     GUINT32 chunk_size = 0,
                     IProgressHandler *ph = 0) const;

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_CRYPTOPP_CRYPTOR_H
