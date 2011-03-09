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

#ifdef GUTIL_ENCRYPTION

#include "encryption.h"
#include "Core/exception.h"
#include "default.h"
#include "hex.h"
#include "base64.h"
#include "files.h"
#include "gzip.h"
#include "osrng.h"
using namespace std;
GUTIL_USING_CORE_NAMESPACE(Utils);

int CryptoHelpers::DEFAULT_COMPRESSION_LEVEL = CryptoPP::Gzip::DEFAULT_DEFLATE_LEVEL;
int CryptoHelpers::MIN_COMPRESSION_LEVEL = CryptoPP::Gzip::MIN_DEFLATE_LEVEL;
int CryptoHelpers::MAX_COMPRESSION_LEVEL = CryptoPP::Gzip::MAX_DEFLATE_LEVEL;

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
    catch(CryptoPP::Exception &ex)
    {
        THROW_NEW_GUTIL_EXCEPTION2( Exception, ex.GetWhat() );
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
    catch(CryptoPP::Exception &ex)
    {
        THROW_NEW_GUTIL_EXCEPTION2( Exception, ex.GetWhat() );
    }

    return outstr;
}

void CryptoHelpers::encryptFile(const char *in, const char *out, const char *passPhrase)
{
    try
    {
        CryptoPP::FileSource f(in, true, new CryptoPP::DefaultEncryptorWithMAC(passPhrase, new CryptoPP::FileSink(out)));
    }
    catch(CryptoPP::Exception &ex)
    {
        THROW_NEW_GUTIL_EXCEPTION2( Exception, ex.GetWhat() );
    }
}

void CryptoHelpers::decryptFile(const char *in, const char *out, const char *passPhrase)
{
    try
    {
        CryptoPP::FileSource f(in, true, new CryptoPP::DefaultDecryptorWithMAC(passPhrase, new CryptoPP::FileSink(out)));
    }
    catch(CryptoPP::Exception &ex)
    {
        THROW_NEW_GUTIL_EXCEPTION2( Exception, ex.GetWhat() );
    }
}

string CryptoHelpers::compress(const string &instr, int level)
{
    string ret;

    bool skip_compression = instr.length() > 10000000;

    if(!skip_compression)
    {
        if(level < MIN_COMPRESSION_LEVEL ||
           level > MAX_COMPRESSION_LEVEL)
            level = DEFAULT_COMPRESSION_LEVEL;

        try
        {
            CryptoPP::Gzip zipper(new CryptoPP::StringSink(ret), level);
            zipper.Put((byte*)instr.c_str(), instr.length());
            zipper.MessageEnd();
        }
        catch(CryptoPP::Exception &ex)
        {
            THROW_NEW_GUTIL_EXCEPTION2( Exception, ex.GetWhat() );
        }
    }

    if(skip_compression ||  ret.length() > instr.length())
    {
        // Leave it uncompressed, because we didn't gain anything by compression
        return "0" + instr;
    }

    return "1" + ret;
}

string CryptoHelpers::decompress(const string &instr)
{
    string tmp;

    if(instr.length() > 0)
    {
        bool is_compressed = false;
        char tmpc = instr.at(0);
        if(tmpc == '1')
            is_compressed = true;

        string newstr;
        if(tmpc == '0' || tmpc == '1')
            newstr = instr.substr(1);
        else
        {
            is_compressed = true;
            newstr = instr;
        }

        if(is_compressed)
        {
            try
            {
                CryptoPP::StringSource(newstr, true, new CryptoPP::Gunzip(new CryptoPP::StringSink(tmp)));
            }
            catch(CryptoPP::Exception &ex)
            {
                THROW_NEW_GUTIL_EXCEPTION2( Exception, ex.GetWhat() );
            }
        }
        else
            return newstr;
    }

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

    try
    {
        CryptoPP::StringSource(instr, true,
                               new CryptoPP::Base64Decoder(new CryptoPP::StringSink(tmp)));
    }
    catch(CryptoPP::Exception &ex)
    {
        THROW_NEW_GUTIL_EXCEPTION2( Exception, ex.GetWhat() );
    }

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

    try
    {
        CryptoPP::StringSource(instr, true,
                               new CryptoPP::HexDecoder(new CryptoPP::StringSink(tmp)));
    }
    catch(CryptoPP::Exception &ex)
    {
        THROW_NEW_GUTIL_EXCEPTION2( Exception, ex.GetWhat() );
    }

    return tmp;
}

unsigned int CryptoHelpers::rand()
{
    return CryptoPP::AutoSeededRandomPool().GenerateWord32();
}

string CryptoHelpers::randData(int size, int seed)
{
    CryptoPP::RandomPool *rng;
    if(seed == -1)
    {
        rng = new CryptoPP::AutoSeededRandomPool();
    }
    else
    {
        rng = new CryptoPP::RandomPool();

        // Seed the random pool
        rng->Put((byte*)(&seed), sizeof(int));
    }

    byte *output = new byte[size];
    rng->GenerateBlock(output, size);

    string ret = string((char*)output, size);
    delete output;
    delete rng;

    return ret;
}








#ifdef GUTIL_COM_EXPORTS

GUTIL_COM_EXPORT int Rand(){ return GUtil::Core::CryptoHelpers::rand(); }

GUTIL_COM_EXPORT std::string EncryptString(const std::string &str, const std::string &password)
{
    return GUtil::Core::CryptoHelpers::encryptString(str, password);
}

GUTIL_COM_EXPORT std::string DecryptString(const std::string &str, const std::string &password)
{
    return GUtil::Core::CryptoHelpers::decryptString(str, password);
}

GUTIL_COM_EXPORT char *ToBase64(const char *str, int len, int &out_len)
{
    std::string ret = GUtil::Core::CryptoHelpers::toBase64(std::string(str, len)).c_str();
    wchar_t *r = new wchar_t[ret.length()];
    memcpy(r, ret.c_str(), ret.length());
    out_len = ret.length();
    return r;
}

GUTIL_COM_EXPORT char *FromBase64(const char *str, int len, int &out_len)
{
    std::string ret = GUtil::Core::CryptoHelpers::fromBase64(std::string(str, len)).c_str();
    char *r = new char[ret.length()];
    memcpy(r, ret.c_str(), ret.length());
    out_len = ret.length();
    return r;
}

#endif // GUTIL_COM_EXPORTS

#endif // GUTIL_ENCRYPTION
