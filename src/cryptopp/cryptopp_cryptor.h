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

#include "gutil_macros.h"

NAMESPACE_GUTIL;

class OutputInterface;


/** Use this class to encrypt and decrypt data */
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
    ~Cryptor();

    /** Returns true if the password is correct, by comparing hashes. */
    bool CheckPassword(const char *) const;

    /** Encrypts the string (the IV for decrypting will be appended to the crypttext)
     *
     *  \param output The crypttext will be pushed here.
    */
    void EncryptData(byte const *plaintext, GUINT32 length, OutputInterface *output);

    /** Decrypts the string. The IV must be appended to the crypttext.
     *  This function has no way to know if decryption fails, so you will get
     *  a random-looking string if it does.
     *
     *  \param output The plaintext will be pushed here.
    */
    void DecryptData(byte const *crypttext, GUINT32 length, OutputInterface *output);


    /** An interface that allows you to handle progress updates, and also cancel the operation. */
    class IProgressHandler
    {
    public:
        /** Called periodically with an updated progress value between 0 (started) and 100 (finished)*/
        virtual void ProgressUpdated(int) = 0;

        /** Called periodically to ask if the operation should be cancelled. Return true if you want to cancel. */
        virtual bool CancelOperation(){ return false; }
    };

    /** Encrypts the file (the IV for decrypting will be appended to the crypttext)
     *
     *  \param output The crypttext will be pushed here.
     *  \param chunk_size Optionally read and encrypt the file in chunks (given in bytes). If you pass 0 it does the whole file in one go.
     *  \param ph A progress handler (pass null if you don't want this feature)
    */
    void EncryptFile(const char *filename, OutputInterface *output, GUINT32 chunk_size = 0, IProgressHandler *ph = 0);

    /** Decrypts the file (the IV for decrypting must be appended to the crypttext)
     *
     *  \param output The plaintext will be pushed here.
     *  \param chunk_size Optionally read and decrypt the file in chunks (given in bytes). If you pass 0 it does the whole file in one go.
     *  \param ph A progress handler (pass null if you don't want this feature)
    */
    void DecryptFile(const char *filename, OutputInterface *output, GUINT32 chunk_size = 0, IProgressHandler *ph = 0);

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_CRYPTOPP_CRYPTOR_H
