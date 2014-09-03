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

#include "gutil_cryptopp_cryptor.h"
#include "gutil_sourcesandsinks.h"
#include "gutil_strings.h"
#include <QtTest>
USING_NAMESPACE_GUTIL;
USING_NAMESPACE_GUTIL1(CryptoPP);

// Keyfiles 1 and 2 differ only in one byte
#define KEYFILE1 "keyfile1.txt"
#define KEYFILE2 "keyfile2.txt"


class CryptorTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void test_basic_encryption();
    void test_encryption_with_authentication();
    void test_encryption_with_keyfiles();
    void test_encryption_with_all_features();
};

void CryptorTest::test_basic_encryption()
{
    Cryptor crypt("password");

    // Test encrypting a small string
    String pData = "Hello world!!!";
    Vector<char> cData;
    {
        ByteArrayInput i(pData.ConstData(), pData.Length());
        VectorByteArrayOutput o(cData);
        crypt.EncryptData(&o, &i);
    }
    QVERIFY(0 != memcmp(pData.ConstData(), cData.ConstData(), pData.Length()));
    QVERIFY(cData.Length() == pData.Length() + crypt.GetNonceSize() + crypt.TagLength);

    Vector<char> recovered;
    try{
        ByteArrayInput i(cData.ConstData(), cData.Length());
        VectorByteArrayOutput o(recovered);
        crypt.DecryptData(&o, &i);
    }
    catch(...){
        QVERIFY(false);
    }
    QVERIFY(0 == memcmp(pData.ConstData(), recovered.ConstData(), pData.Length()));
    QVERIFY(pData == recovered);

    // Encrypting the same thing twice gives you completely different crypttexts
    Vector<char> cData2;
    {
        ByteArrayInput i(pData.ConstData(), pData.Length());
        VectorByteArrayOutput o(cData2);
        crypt.EncryptData(&o, &i);
    }
    QVERIFY(cData.Length() == cData2.Length());
    QVERIFY(0 != memcmp(cData.ConstData(), cData2.ConstData(), cData.Length()));

    // Test encrypting a long string
    cData.Empty();
    pData = "This is a really long string, to see if it makes a difference"
            " between a long string and a short string. It might actually, because"
            " you know, if the message is longer than a single block of the"
            " block cipher then this tests the cryptor's ability to chain the"
            " data blocks and the cipher correctly.";
    {
        ByteArrayInput i(pData.ConstData(), pData.Length());
        VectorByteArrayOutput o(cData);
        crypt.EncryptData(&o, &i);
    }
    QVERIFY(0 != memcmp(pData.ConstData(), cData.ConstData(), pData.Length()));

    recovered.Empty();
    try{
        ByteArrayInput i(cData.ConstData(), cData.Length());
        VectorByteArrayOutput o(recovered);
        crypt.DecryptData(&o, &i);
    }
    catch(...){
        QVERIFY(false);
    }
    QVERIFY(pData == recovered);

    // Try decrypting with a bad password
    Cryptor crypt2("badpassword");
    bool exception_hit = false;
    try{
        ByteArrayInput i(cData.ConstData(), cData.Length());
        crypt2.DecryptData(NULL, &i);
    }
    catch(const AuthenticationException<> &){
        exception_hit = true;
    }
    catch(...){
        QVERIFY(false);
    }
    QVERIFY(exception_hit);

    // Try corrupting the message by incrementing a byte somewhere
    cData[0] = cData[0] + 1;
    exception_hit = false;
    try{
        ByteArrayInput i(cData.ConstData(), cData.Length());
        crypt.DecryptData(NULL, &i);
    }
    catch(const AuthenticationException<> &){
        exception_hit = true;
    }
    catch(...){
        QVERIFY(false);
    }
    QVERIFY(exception_hit);
}

void CryptorTest::test_encryption_with_authentication()
{
    Cryptor crypt("password");

    String pData = "Hello world!!!";
    String aData = "This message will be authenticated";
    Vector<char> cData;

    // First authenticate a message without pData
    {
        ByteArrayInput i(aData.ConstData(), aData.Length());
        VectorByteArrayOutput o(cData);
        crypt.EncryptData(&o, NULL, &i);
    }
    // The crypttext has only the IV and MAC
    QVERIFY(cData.Length() == crypt.GetNonceSize() + crypt.TagLength);

    // Verify the authenticity correctly
    try{
        ByteArrayInput i(cData.ConstData(), cData.Length());
        ByteArrayInput ia(aData.ConstData(), aData.Length());
        crypt.DecryptData(NULL, &i, &ia);
    }
    catch(...){
        QVERIFY(false);
    }

    // Corrupt the message by truncating the auth data, authenticity check fails
    bool exception_hit = false;
    try{
        ByteArrayInput i(cData.ConstData(), cData.Length());
        ByteArrayInput ia(aData.ConstData(), aData.Length() - 1);
        crypt.DecryptData(NULL, &i, &ia);
    }
    catch(const AuthenticationException<> &){
        exception_hit = true;
    }
    catch(...){
        QVERIFY(false);
    }
    QVERIFY(exception_hit);


    // Now try encrypting with both pData and aData
    cData.Empty();
    {
        ByteArrayInput i(pData.ConstData(), pData.Length());
        ByteArrayInput ia(aData.ConstData(), aData.Length());
        VectorByteArrayOutput o(cData);
        crypt.EncryptData(&o, &i, &ia);
    }
    // The crypttext has the plaintext plus IV and MAC
    QVERIFY(cData.Length() == pData.Length() + crypt.GetNonceSize() + crypt.TagLength);

    // Recover the pData and verify the authenticity
    Vector<char> recovered;
    try{
        ByteArrayInput i(cData.ConstData(), cData.Length());
        ByteArrayInput ia(aData.ConstData(), aData.Length());
        VectorByteArrayOutput o(recovered);
        crypt.DecryptData(&o, &i, &ia);
    }
    catch(...){
        QVERIFY(false);
    }
    QVERIFY(pData.Length() == recovered.Length());
    QVERIFY(0 == memcmp(pData.ConstData(), recovered.ConstData(), pData.Length()));

    // Authenticity check fails if auth data is different
    exception_hit = false;
    recovered.Empty();
    try{
        ByteArrayInput i(cData.ConstData(), cData.Length());
        ByteArrayInput ia(aData.ConstData(), aData.Length() - 1);
        crypt.DecryptData(NULL, &i, &ia);
    }
    catch(const AuthenticationException<> &){
        exception_hit = true;
    }
    catch(...){
        QVERIFY(false);
    }
    QVERIFY(exception_hit);
}

void CryptorTest::test_encryption_with_keyfiles()
{
    // First let's make one successfully so we feel good about ourselves
    try{
        Cryptor(NULL, KEYFILE1);
    }
    catch(...){
        QVERIFY(false);
    }

    // Now let's try to break it by giving it bad inputs
    // The keyfile must exist
    bool exception_hit = false;
    try{
        Cryptor(NULL, "this_file_doesnt_exist");
    }
    catch(...){
        exception_hit = true;
    }
    QVERIFY(exception_hit);

    // can't give it a directory
    exception_hit = false;
    try{
        Cryptor(NULL, ".");
    }
    catch(...){
        exception_hit = true;
    }
    QVERIFY(exception_hit);

    // Ok now let's encrypt and decrypt and see if it works using keyfiles
    Cryptor crypt(NULL, KEYFILE1);
    String pData = "Hello world!!!";
    Vector<char> cData;
    {
        ByteArrayInput i(pData.ConstData(), pData.Length());
        VectorByteArrayOutput o(cData);
        crypt.EncryptData(&o, &i);
    }
    QVERIFY(cData.Length() == pData.Length() + crypt.GetNonceSize() + crypt.TagLength);
    QVERIFY(0 != memcmp(pData.ConstData(), cData.ConstData(), pData.Length()));

    Vector<char> recovered;
    try{
        ByteArrayInput i(cData.ConstData(), cData.Length());
        VectorByteArrayOutput o(recovered);
        crypt.DecryptData(&o, &i);
    }
    catch(...){
        QVERIFY(false);
    }
    QVERIFY(0 == memcmp(pData.ConstData(), recovered.ConstData(), pData.Length()));
    QVERIFY(pData == recovered);

    // What if we decrypt with the wrong keyfile?
    Cryptor crypt2(NULL, KEYFILE2);
    exception_hit = false;
    try{
        ByteArrayInput i(cData.ConstData(), cData.Length());
        crypt2.DecryptData(NULL, &i);
    }
    catch(const AuthenticationException<> &){
        exception_hit = true;
    }
    catch(...){
        QVERIFY(false);
    }
    QVERIFY(exception_hit);

    // What if we decrypt with the right keyfile but also give a password?
    crypt2.ChangePassword("password", KEYFILE1);
    exception_hit = false;
    try{
        ByteArrayInput i(cData.ConstData(), cData.Length());
        crypt2.DecryptData(NULL, &i);
    }
    catch(const AuthenticationException<> &){
        exception_hit = true;
    }
    catch(...){
        QVERIFY(false);
    }
    QVERIFY(exception_hit);

    // But after everything we can still decrypt it with the right keyfile
    crypt2.ChangePassword("", KEYFILE1);
    recovered.Empty();
    try{
        ByteArrayInput i(cData.ConstData(), cData.Length());
        VectorByteArrayOutput o(recovered);
        crypt2.DecryptData(&o, &i);
    }
    catch(...){
        QVERIFY(false);
    }
    QVERIFY(0 == memcmp(pData.ConstData(), recovered.ConstData(), pData.Length()));
    QVERIFY(pData == recovered);
}

void CryptorTest::test_encryption_with_all_features()
{
    Cryptor crypt("password", KEYFILE1);

    // Test basic encryption with auth data
    String pData = "Hello world!!!";
    String aData = "This data will be authenticated!";
    Vector<char> cData;
    {
        ByteArrayInput i(pData.ConstData(), pData.Length());
        ByteArrayInput ia(aData.ConstData(), aData.Length());
        VectorByteArrayOutput o(cData);
        crypt.EncryptData(&o, &i, &ia);
    }
    QVERIFY(cData.Length() == pData.Length() + crypt.GetNonceSize() + crypt.TagLength);
    QVERIFY(0 != memcmp(pData.ConstData(), cData.ConstData(), pData.Length()));

    Vector<char> recovered;
    try{
        ByteArrayInput i(cData.ConstData(), cData.Length());
        ByteArrayInput ia(aData.ConstData(), aData.Length());
        VectorByteArrayOutput o(recovered);
        crypt.DecryptData(&o, &i, &ia);
    }
    catch(...){
        QVERIFY(false);
    }
    QVERIFY(0 == memcmp(pData.ConstData(), recovered.ConstData(), pData.Length()));
    QVERIFY(pData == recovered);

    // Try decrypting with a bad password but bood keyfile
    crypt.ChangePassword("pasSword", KEYFILE1);
    bool exception_hit = false;
    try{
        ByteArrayInput i(cData.ConstData(), cData.Length());
        ByteArrayInput ia(aData.ConstData(), aData.Length());
        crypt.DecryptData(NULL, &i, &ia);
    }
    catch(const AuthenticationException<> &){
        exception_hit = true;
    }
    catch(...){
        QVERIFY(false);
    }
    QVERIFY(exception_hit);

    // Try decrypting with a good password but bad keyfile
    crypt.ChangePassword("password", KEYFILE2);
    exception_hit = false;
    try{
        ByteArrayInput i(cData.ConstData(), cData.Length());
        ByteArrayInput ia(aData.ConstData(), aData.Length());
        crypt.DecryptData(NULL, &i, &ia);
    }
    catch(const AuthenticationException<> &){
        exception_hit = true;
    }
    catch(...){
        QVERIFY(false);
    }
    QVERIFY(exception_hit);

    // Now with the right password and keyfile it should work again
    crypt.ChangePassword("password", KEYFILE1);
    recovered.Empty();
    try{
        ByteArrayInput i(cData.ConstData(), cData.Length());
        ByteArrayInput ia(aData.ConstData(), aData.Length());
        VectorByteArrayOutput o(recovered);
        crypt.DecryptData(&o, &i, &ia);
    }
    catch(...){
        QVERIFY(false);
    }
    QVERIFY(0 == memcmp(pData.ConstData(), recovered.ConstData(), pData.Length()));
    QVERIFY(pData == recovered);

    // If the auth data was corrupted it should also fail
    exception_hit = false;
    try{
        ByteArrayInput i(cData.ConstData(), cData.Length());
        ByteArrayInput ia(aData.ConstData(), aData.Length() - 1);
        crypt.DecryptData(NULL, &i, &ia);
    }
    catch(const AuthenticationException<> &){
        exception_hit = true;
    }
    catch(...){
        QVERIFY(false);
    }
    QVERIFY(exception_hit);

    // If the crypttext was corrupted it should also fail
    cData[15] = ~cData[15];
    exception_hit = false;
    try{
        ByteArrayInput i(cData.ConstData(), cData.Length());
        ByteArrayInput ia(aData.ConstData(), aData.Length());
        crypt.DecryptData(NULL, &i, &ia);
    }
    catch(const AuthenticationException<> &){
        exception_hit = true;
    }
    catch(...){
        QVERIFY(false);
    }
    QVERIFY(exception_hit);
}

QTEST_APPLESS_MAIN(CryptorTest)

#include "tst_cryptortest.moc"
