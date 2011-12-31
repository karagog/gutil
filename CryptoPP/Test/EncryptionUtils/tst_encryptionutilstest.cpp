/*Copyright 2011 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "Core/DataObjects/gstring.h"
#include "CryptoPP/encryptionutils.h"
#include <QtCore/QString>
#include <QtTest/QtTest>
USING_NAMESPACE_GUTIL1(DataObjects);
USING_NAMESPACE_GUTIL1(CryptoPP);
USING_NAMESPACE_GUTIL;

class EncryptionUtilsTest : public QObject
{
    Q_OBJECT
    
public:
    EncryptionUtilsTest();
    
private Q_SLOTS:
    void test_random_string();
    void test_compression();
    void test_encryption();
    void test_hash();
};

EncryptionUtilsTest::EncryptionUtilsTest()
{
}

void EncryptionUtilsTest::test_compression()
{
    String s("Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello");
    String compressed( EncryptionUtils::CompressString(s) );
    String decompressed( EncryptionUtils::DecompressString(compressed) );

//    qDebug() << s.ToBase64().ConstData();
//    qDebug() << compressed.ToBase64().ConstData();
//    qDebug() << decompressed.ToBase64().ConstData();

    QVERIFY2(s == "Hello Hello Hello Hello Hello Hello Hello Hello Hello Hello", s);
    QVERIFY(s != compressed);
    QVERIFY2(s.Length() > compressed.Length(), String::Format("%d != %d", s.Length(), compressed.Length()));
    QVERIFY2(s == decompressed, String::Format("%s != %s", s.ConstData(), decompressed.ConstData()));
}

void EncryptionUtilsTest::test_encryption()
{
    // Test the des encryptor
    String password = "I like Toast!";
    String plaintext("Hello World, my name is George!");
    String ciphertext = EncryptionUtils::EncryptString(plaintext, password, EncryptionUtils::DES_Encryption);
    String decrypted = EncryptionUtils::DecryptString(ciphertext, password, EncryptionUtils::DES_Encryption);

//    qDebug(plaintext);
//    qDebug(plaintext.ToBase16());
//    qDebug(ciphertext.ToBase16());
//    qDebug(decrypted.ToBase16());

    QVERIFY(plaintext != ciphertext);
    QVERIFY2(decrypted == plaintext, String::Format("%s != %s", decrypted.ToBase16().ConstData(), plaintext.ToBase16().ConstData()));


    // Test Triple-DES
    ciphertext = EncryptionUtils::EncryptString(plaintext, password, EncryptionUtils::TripleDES_Encryption);
    decrypted = EncryptionUtils::DecryptString(ciphertext, password, EncryptionUtils::TripleDES_Encryption);
    QVERIFY(plaintext != ciphertext);
    QVERIFY2(decrypted == plaintext, String::Format("%s != %s", decrypted.ToBase16().ConstData(), plaintext.ToBase16().ConstData()));


    // Test AES encryption
    ciphertext = EncryptionUtils::EncryptString(plaintext, password, EncryptionUtils::AES_Encryption);
    decrypted = EncryptionUtils::DecryptString(ciphertext, password, EncryptionUtils::AES_Encryption);
    QVERIFY(plaintext != ciphertext);
    QVERIFY2(decrypted == plaintext, String::Format("%s != %s", decrypted.ToBase16().ConstData(), plaintext.ToBase16().ConstData()));

//    qDebug(plaintext);
//    qDebug(plaintext.ToBase16());
//    qDebug(ciphertext.ToBase16());
//    qDebug(decrypted.ToBase16());


    // Set up the exception case tests, by encrypting with DES_EDE2
    ciphertext = EncryptionUtils::EncryptString(plaintext, password, EncryptionUtils::DES_Encryption);

    bool ex_hit(false);
    try
    {
        // Try decrypting with a bad password
        decrypted = EncryptionUtils::DecryptString(ciphertext, "Bad Password");
    }
    catch(const Exception<> &)
    {
        ex_hit = true;
    }
    QVERIFY(ex_hit);


    ex_hit = false;
    try
    {
        // Try to decrypt with Triple DES
        decrypted = EncryptionUtils::DecryptString(ciphertext, password, EncryptionUtils::TripleDES_Encryption);
    }
    catch(const Exception<> &)
    {
        ex_hit = true;
    }
    QVERIFY(ex_hit);

    ex_hit = false;
    try
    {
        // Try to decrypt with AES
        decrypted = EncryptionUtils::DecryptString(ciphertext, password, EncryptionUtils::AES_Encryption);
    }
    catch(const Exception<> &)
    {
        ex_hit = true;
    }
    QVERIFY(ex_hit);


    ciphertext = EncryptionUtils::EncryptString(plaintext, password, EncryptionUtils::AES_Encryption);
    ex_hit = false;
    try
    {
        decrypted = EncryptionUtils::DecryptString(ciphertext, password, EncryptionUtils::TripleDES_Encryption);
    }
    catch(const Exception<> &)
    {
        ex_hit = true;
    }
    QVERIFY(ex_hit);
}

void EncryptionUtilsTest::test_random_string()
{
    const GUINT32 slen( 12 );
    const GUINT32 seed( 0 );    // Seeding just adds entropy to the random numbers

    String s1 = EncryptionUtils::RandomString(slen, seed);
    String s2 = EncryptionUtils::RandomString(slen, seed);

//    qDebug(s1.ToBase16());
//    qDebug(s2.ToBase16());

    QVERIFY(s1.Length() == slen);
    QVERIFY(s2.Length() == slen);
    QVERIFY(s1[s1.Length()] == '\0');
    QVERIFY(s2[s2.Length()] == '\0');
    QVERIFY(s1 != s2);
}

void EncryptionUtilsTest::test_hash()
{
    // Try hashing an empty string (hash references from Wikipedia)
    String s;
    QVERIFY(EncryptionUtils::Hash(s, EncryptionUtils::MD5Hash).ToBase16().ToLower() == "d41d8cd98f00b204e9800998ecf8427e");
    QVERIFY(EncryptionUtils::Hash(s, EncryptionUtils::MD4Hash).ToBase16().ToLower() ==     "31d6cfe0d16ae931b73c59d7e0c089c0");
    QVERIFY(EncryptionUtils::Hash(s, EncryptionUtils::SHA1Hash).ToBase16().ToLower() ==    "da39a3ee5e6b4b0d3255bfef95601890afd80709");
    QVERIFY(EncryptionUtils::Hash(s, EncryptionUtils::SHA224Hash).ToBase16().ToLower() ==  "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f");
    QVERIFY(EncryptionUtils::Hash(s, EncryptionUtils::SHA256Hash).ToBase16().ToLower() ==  "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    QVERIFY(EncryptionUtils::Hash(s, EncryptionUtils::SHA384Hash).ToBase16().ToLower() ==  "38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b");
    QVERIFY(EncryptionUtils::Hash(s, EncryptionUtils::SHA512Hash).ToBase16().ToLower() ==  "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e");


    // Try some reference hashes
    QVERIFY(EncryptionUtils::Hash("The quick brown fox jumps over the lazy dog", UINT_MAX, EncryptionUtils::MD5Hash).ToBase16().ToLower()
            == "9e107d9d372bb6826bd81d3542a419d6");
    QVERIFY(EncryptionUtils::Hash("The quick brown fox jumps over the lazy dog", UINT_MAX, EncryptionUtils::MD4Hash).ToBase16().ToLower()
            == "1bee69a46ba811185c194762abaeae90");
    QVERIFY(EncryptionUtils::Hash("The quick brown fox jumps over the lazy dog", UINT_MAX, EncryptionUtils::SHA1Hash).ToBase16().ToLower()
            == "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12");
    QVERIFY(EncryptionUtils::Hash("The quick brown fox jumps over the lazy dog", UINT_MAX, EncryptionUtils::SHA224Hash).ToBase16().ToLower()
            == "730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525");
    QVERIFY(EncryptionUtils::Hash("The quick brown fox jumps over the lazy dog", UINT_MAX, EncryptionUtils::SHA256Hash).ToBase16().ToLower()
            == "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592");
    QVERIFY(EncryptionUtils::Hash("The quick brown fox jumps over the lazy dog", UINT_MAX, EncryptionUtils::SHA384Hash).ToBase16().ToLower()
            == "ca737f1014a48f4c0b6dd43cb177b0afd9e5169367544c494011e3317dbf9a509cb1e5dc1e85a941bbee3d7f2afbc9b1");
    QVERIFY(EncryptionUtils::Hash("The quick brown fox jumps over the lazy dog", UINT_MAX, EncryptionUtils::SHA512Hash).ToBase16().ToLower()
            == "07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb642e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6");
}

QTEST_APPLESS_MAIN(EncryptionUtilsTest)

#include "tst_encryptionutilstest.moc"
