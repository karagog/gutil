/*Copyright 2010 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "encryption.h"
#include "default.h"
#include "hex.h"
#include "base64.h"
#include "files.h"
#include "gzip.h"
using namespace std;
using namespace GUtil;

string CryptoHelpers::encryptString(const string &instr, const string &passPhrase)
{
    string outstr;

    CryptoPP::DefaultEncryptorWithMAC encryptor(passPhrase.c_str(),
                                                new CryptoPP::StringSink(outstr));

    try
    {
        encryptor.Put((byte *)instr.c_str(), instr.length());
        encryptor.MessageEnd();
    }
    catch(CryptoPP::Exception ex)
    {
        return "";
    }

    return outstr;
}

string CryptoHelpers::decryptString(const string &instr, const string &passPhrase)
{
    string outstr;

    CryptoPP::DefaultDecryptorWithMAC decryptor(passPhrase.c_str(),
                                                new CryptoPP::StringSink(outstr));

    try
    {
        decryptor.Put((byte *)instr.c_str(), instr.length());
        decryptor.MessageEnd();
    }
    catch(CryptoPP::Exception ex)
    {
        return "";
    }

    return outstr;
}

bool CryptoHelpers::encryptFile(const char *in, const char *out, const char *passPhrase)
{
    try
    {
        CryptoPP::FileSource f(in, true, new CryptoPP::DefaultEncryptorWithMAC(passPhrase, new CryptoPP::FileSink(out)));
    }
    catch(CryptoPP::Exception ex)
    {
        return false;
    }

    return true;
}

bool CryptoHelpers::decryptFile(const char *in, const char *out, const char *passPhrase)
{
    try
    {
        CryptoPP::FileSource f(in, true, new CryptoPP::DefaultDecryptorWithMAC(passPhrase, new CryptoPP::FileSink(out)));
    }
    catch(CryptoPP::Exception ex)
    {
        return false;
    }

    return true;
}

string CryptoHelpers::compress(const string &instr)
{
    string ret;

    CryptoPP::Gzip zipper(new CryptoPP::StringSink(ret));

    zipper.Put((byte*)instr.c_str(), instr.length());
    zipper.MessageEnd();

//    byte *tmpbytes = new byte[zipper.MaxRetrievable()];
//    zipper.Get(tmpbytes, zipper.MaxRetrievable());

//    string ret((char*)tmpbytes, zipper.MaxRetrievable());
//    delete tmpbytes;

    return ret;
}

string CryptoHelpers::decompress(const string &instr)
{
    string tmp;

    CryptoPP::StringSource(instr, true, new CryptoPP::Gunzip(new CryptoPP::StringSink(tmp)));

    return tmp;
}

string CryptoHelpers::toBase64(const string &instr)
{
    string tmp;

    CryptoPP::Base64Encoder encoder(new CryptoPP::StringSink(tmp), false);
    encoder.Put((byte *)instr.c_str(), instr.length());
    encoder.MessageEnd();

    return tmp;
}

string CryptoHelpers::fromBase64(const string &instr)
{
    string tmp;

    CryptoPP::StringSource(instr, true,
                           new CryptoPP::Base64Decoder(new CryptoPP::StringSink(tmp)));

    return tmp;
}

string CryptoHelpers::toBase16(const string &instr)
{
    string tmp;

    CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(tmp), true);
    encoder.Put((byte *)instr.c_str(), instr.length());
    encoder.MessageEnd();

    return tmp;
}

string CryptoHelpers::fromBase16(const string &instr)
{
    string tmp;

    CryptoPP::StringSource(instr, true,
                           new CryptoPP::HexDecoder(new CryptoPP::StringSink(tmp)));

    return tmp;
}
