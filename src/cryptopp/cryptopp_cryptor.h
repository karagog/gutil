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

#ifndef GUTIL_CRYPTOPP_CRYPTOR_H
#define GUTIL_CRYPTOPP_CRYPTOR_H

#include <gutil/iio.h>
#include <gutil/iprogresshandler.h>
#include <gutil/iclonable.h>
#include <gutil/vector.h>
#include <gutil/smartpointer.h>

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
 *  You can change your key by changing any or all of the password, keyfile or salt.
*/
class Cryptor
{
public:
    class IKeyDerivation;

    /** The size of the tag, or Message Authentication Code (MAC) that goes on
     *  every encrypted message. The MAC is used to verify authenticity of the
     *  entire message, so if any single bit is wrong the MAC tag will know it.
     *
     *  This contributes as many bytes to the size of the resulting crypttext.
    */
    static const GUINT32 TagLength = 16;

    /** The default nonce size. */
    static const GUINT8 DefaultNonceSize = 10;

    /** The size of the cryptographic key, in bytes. */
    static const GUINT32 KeySize = 32;

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
     *  \param nonce_size The length of the nonce, in bytes. Valid values are between 7 and 13
     *  \param keyfile The key file, if any. Can be any file of any size, but it's better
     *      if it's small (under or around 100 bytes) and random. If you leave this blank, only the
     *      password will be required to decrypt.
     *  \param salt A byte array to initialize the hash functions that generate keys from passwords.
     *              You need this to keep your keys fresh without the user changing passwords.
     *  \param kdf Allows you to supply your own custom key derivation function. The default KDF should
     *              be plenty good, but this allows you more flexibility. The cryptor owns the KDF and deletes it.
    */
    Cryptor(const char *password, const char *keyfile = 0,
            GUINT8 nonce_size = DefaultNonceSize,
            IKeyDerivation *kdf = new DefaultKeyDerivation);

    /** Duplicates the cryptor, taking on the other's password. */
    Cryptor(const Cryptor &);
    ~Cryptor();

    GUINT8 GetNonceSize() const{ return m_nonceSize; }

    /** Returns the theoretical maximum payload length as a function of nonce size. */
    static GUINT64 GetMaxPayloadLength(GUINT8 nonce_size);

    /** Returns the maximum nonce size required to be able to encrypt a payload of the given length. */
    static GUINT8 GetNonceSizeRequired(GUINT64 max_payload);

    /** Returns the maximum suggested number of key uses for the given nonce length.
     *  This function assumes random nonce generation. If you are deterministically
     *  producing nonces with a counter you can get far more uses (2^8n, where n is the nonce size),
     *  just as long as you never repeat a nonce with the same key.
     *
     *  It satisfies the requirement that the probability of generating a random nonce that
     *  has appeared before is less than 2^-32.
     *
     *  \returns The maximum number of key invocations as a suggestion, assuming random nonce generation.
    */
    static double GetMaxKeyUsageSuggestion(GUINT8 nonce_length);

    /** Returns true if the password/keyfile combination is correct. */
    bool CheckPassword(const char *password, const char *keyfile = 0) const;

    /** Changes the password/keyfile combination used by the cryptor. */
    void ChangePassword(const char *password, const char *keyfile = 0);


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
     *  \param nonce The nonce, if you choose to provide it, must be GetNonceSize() bytes. If you
     *          pass null a nonce will be randomly generated for you.
     *  \param chunk_size Optionally process the file in chunks (given in bytes).
     *          If you pass 0 it does the whole file in one go.
     *  \param ph An optional progress handler for long operations; large data sources.
     *          In order for this to be useful, you should set a reasonable value for chunk_size.
    */
    void EncryptData(GUtil::IOutput *out,
                     GUtil::IInput *pData = NULL,
                     GUtil::IInput *aData = NULL,
                     byte const *nonce = NULL,
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


    /** Defines an interface to allow you to define your own password-based key derivation functions.
        This is for advanced users only; normally you don't have to use this.
    */
    class IKeyDerivation : public GUtil::IClonable
    {
    public:
        /** Create the key from a password. key_out must be Cryptor::KeySize large. */
        virtual void DeriveKeyFromPassword(byte *key_out, const char *password) = 0;

        /** Create the key from a keyfile. key_out must be Cryptor::KeySize large. */
        virtual void DeriveKeyFromKeyfile(byte *key_out, const char *keyfile) = 0;

        /** Create the secret auth data from a password. 
            It must be at least greater than 0, but it's better if it's longer.
        */
        virtual GUtil::Vector<byte> DeriveAuthData(const char *password) = 0;

        /** You will be deleted by this interface. */
        virtual ~IKeyDerivation() {}
    };
    
    /** The default key derivation functions for the Cryptor. */
    class DefaultKeyDerivation : public IKeyDerivation
    {
        const GUtil::Vector<byte> m_salt;
    public:
        /** It is optional (but highly recommended) to supply salt for deriving the key.
            This salt guarantees that you get a unique key even if you don't change the password,
            and forces a dictionary attacker to regenerate his dictionary using the salt, which is
            (by design) a very expensive process.
        */
        DefaultKeyDerivation(byte const *salt, GUINT32 salt_len) :m_salt(salt, salt_len, true) {}
        DefaultKeyDerivation() {}
        DefaultKeyDerivation(const DefaultKeyDerivation &o) :m_salt(o.m_salt, true) {}
        virtual GUtil::IClonable *Clone() const noexcept;

        /** Key is derived by iteratively hashing the password, and using every
            successive hash as salt for the next iteration.
            
            This is designed to be a slow operation, to make it prohibitively expensive for
            an attacker to generate a dictionary of password hashes, but still fast enough
            that it doesn't detract from the user's experience if they just need to generate 1 key.
        */
        virtual void DeriveKeyFromPassword(byte *, const char *);

        /** Key is derived by straightforward hash. Dictionary attacks don't apply
         *  for keyfiles.
        */
        virtual void DeriveKeyFromKeyfile(byte *, const char *);

        /** Auth data is derived by iteratively hashing the password, and using every
            successive hash as salt for the next iteration.
         
            This is designed to be a slow operation, to make it prohibitively expensive for
            an attacker to generate a dictionary of password hashes, but still fast enough
            that it doesn't detract from the user's experience if they just need to generate 1 key.
        */
        virtual GUtil::Vector<byte> DeriveAuthData(const char *);
    };


private:

    const GUINT8 m_nonceSize;
    byte m_key[KeySize];
    GUtil::Vector<byte> m_authData;
    GUtil::SmartPointer<IKeyDerivation> m_kdf;
    void *d;

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_CRYPTOPP_CRYPTOR_H
