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

    /** Encrypts the file (the IV for decrypting will be appended to the crypttext)
     *
     *  \param output The crypttext will be pushed here.
    */
    void EncryptFile(const char *filename, OutputInterface *output);

    /** Decrypts the file (the IV for decrypting must be appended to the crypttext)
     *
     *  \param output The plaintext will be pushed here.
    */
    void DecryptFile(const char *filename, OutputInterface *output);

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_CRYPTOPP_CRYPTOR_H
