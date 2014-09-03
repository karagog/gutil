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
#include "gutil_iprogresshandler.h"

NAMESPACE_GUTIL1(CryptoPP);


/** Use this class to encrypt and decrypt data securely and with message authentication.
 *
 *  Authentication means that on decrypting it will recognize if the message has been
 *  tampered with or corrupted. This assurance comes at a small cost of 16 additional bytes
 *  added onto every encrypted message.
 *
 *  The crypttext output of all functions will contain a 16-byte
 *  Message Authentication Code (MAC) and the Nonce of configurable size that are
 *  required to decrypt the message. The total size of the crypttext will be the size of
 *  the plaintext data plus the MAC and Nonce.
 *
 *  You can optionally supply authenticated data to the encryption functions, which will
 *  not add to the size of the crypttext (you will transmit this data in clear text),
 *  but it will nonetheless be required to decrypt the message.
 *
 *  A note on limits:
 *  The encryption method is AES-CCM. With CCM mode, the maximum payload size is
 *  configurable based on the size of the Nonce, since you must specify the length
 *  of the data before encryption and it must fit inside a 15-byte block with the nonce
 *  itself. Therefore, whatever the nonce length (N) is, you have 15-N bytes to represent
 *  the payload size. However, before you decide you want a minimum sized nonce to maximize
 *  your payload, the nonce must be guaranteed to be unique every time you use the key, with
 *  the probability of collision less than 2^-32. So if your nonce is 7 bytes (the minimum)
 *  that is 2^56 possible nonces, and if you're generating them randomly then after 2^24
 *  (about 16 million) encryptions you will exceed this threshold and have to change your key.
*/
class Cryptor
{
    void *d;
    GUINT8 m_nonceSize;
    byte m_key[32];     // The max key size for AES
    byte m_key2[64];    // The size of a SHA512 hash
public:

    /** The size of the tag, or Message Authentication Code (MAC) that goes on
     *  every encrypted message. The MAC is used to verify authenticity of the
     *  entire message, so if any single bit is wrong the MAC tag will know it.
     *
     *  This contributes as many bytes to the size of the resulting crypttext.
    */
    static const GUINT32 TagLength = 16;

    /** Constructs and initializes the Cryptor using the password and/or keyfile.
     *
     *  If you supply a password, then only the password will be required to decrypt. If you
     *  supply a keyfile but not a password, then only the keyfile will be required to decrypt.
     *
     *  The cryptor implements multifactor authentication, if you want to use it by supplying both
     *  a password and a keyfile. If you do this, you will be required to present both the correct
     *  password and keyfile if you want to successfully decrypt the message.
     *
     *  \param password The password to be used for encryption/decryption, must be in UTF-8/ASCII format.
     *      After initialization, the Cryptor has no idea what the password is anymore, but you can check if
     *      another one matches by using the CheckPassword function.
     *  \param keyfile The key file, if any. Can be any file of any size, but it's better
     *      if it's small (under or around 100 bytes) and random. If you leave this blank, only the
     *      password will be required to decrypt.
     *  \param salt A byte array to initialize the hash functions that generate keys from passwords.
     *              You need this to keep your keys fresh without the user changing passwords.
     *  \param nonce_length The length of the nonce, in bytes. Valid values are between 7 and 13
    */
    Cryptor(const char *password, const char *keyfile = 0,
            const byte *salt = NULL, GUINT32 salt_len = 0,
            GUINT8 nonce_length = 10);

    /** Duplicates the cryptor, taking on the other's password. */
    Cryptor(const Cryptor &);
    ~Cryptor();

    /** The length of the nonce. This determines several critical properties
     *  of the cryptor.
     *
     *  There is not a one nonce-size fits all for this parameter. You should
     *  tailor this number to your needs, depending on the size of the payloads you will
     *  be encrypting and the number of times the key will be reused.
     *
     *  \param size The length of the nonce, in bytes. Valid values are between 7 and 13
     *  \throws An exception if the value is out of range
    */
    void SetNonceSize(GUINT8 size);

    /** Returns the nonce size
     *  \sa SetNonceSize()
    */
    GUINT8 GetNonceSize() const{ return m_nonceSize; }
    
    /** Returns the maximum payload length, which is a function of the nonce size. */
    GUINT64 GetMaxPayloadLength() const;
    
    /** Sets the nonce size such that the maximum payload length will be at least as great or greater
        than the length you specify here. Any value is valid.
    */
    void SetMaxPayloadLength(GUINT64);

    /** Returns true if the password/keyfile combination is correct. */
    bool CheckPassword(const char *password, const char *keyfile = 0,
                       const byte *salt = NULL, GUINT32 salt_len = 0) const;

    /** Changes the password/keyfile combination used by the cryptor. */
    void ChangePassword(const char *password, const char *keyfile = 0,
                        const byte *salt = NULL, GUINT32 salt_len = 0);


    /** Encrypts the string.
     *
     *  \param out The crypttext will be pushed here.
     *  \param pData The plaintext data that will be encrypted AND authenticated. This may be
     *          null, in which case you will get a MAC as output that can be used to validate the password.
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
    void EncryptData(GUtil::IOutput *out,
                     GUtil::IInput *pData = NULL,
                     GUtil::IInput *aData = NULL,
                     GUINT32 chunk_size = 0,
                     GUtil::IProgressHandler *ph = NULL);

    /** Decrypts the string and throw an exception if decryption failed.
     *  It could fail due to a wrong key, wrong data (length), or if a bit was flipped somewhere
     *  either maliciously or accidentally, or the initialization vector wasn't right.
     *
     *  \param out The recovered plaintext will be pushed here.
     *              This paramater may be null, in which case you won't get the plaintext, but
     *              you can still validate the authenticity of the message.
     *  \param cData The crypttext data that was produced by EncryptData.
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
    void DecryptData(GUtil::IOutput *out,
                     GUtil::IRandomAccessInput *cData,
                     GUtil::IInput *aData = NULL,
                     GUINT32 chunk_size = 0,
                     GUtil::IProgressHandler *ph = NULL);

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_CRYPTOPP_CRYPTOR_H
