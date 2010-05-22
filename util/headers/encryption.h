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

#ifndef CRYPTOHELPERS_H
#define CRYPTOHELPERS_H
#include <string>

using namespace std;

namespace GUtil
{

    class CryptoHelpers
    {
    public:
        // For working with strings (returns null if conversion fails)
        static string encryptString(const string &instr, const string &passPhrase);
        static string decryptString(const string &instr, const string &passPhrase);

        // For working with files
        static bool encryptFile(const char *in, const char *out, const char *passPhrase);
        static bool decryptFile(const char *in, const char *out, const char *passPhrase);

        // Data Compression functions
        static string compress(const string &instr, int level = DEFAULT_COMPRESSION_LEVEL);
        static string decompress(const string &instr);

        static int DEFAULT_COMPRESSION_LEVEL;
        static int MIN_COMPRESSION_LEVEL;
        static int MAX_COMPRESSION_LEVEL;

        // Different data encoding schemes
        static string toBase64(const string &instr);
        static string fromBase64(const string &instr);

        static string toBase16(const string &instr);
        static string fromBase16(const string &instr);

        // Random number generator
        static unsigned int rand();
        static string randData(int, int seed = -1);


    };
}


#endif // CRYPTOHELPERS_H
