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

#ifdef GUTIL_ENCRYPTION

#ifndef CRYPTOHELPERS_H
#define CRYPTOHELPERS_H

#include "gutil.h"
#include <string>

GUTIL_BEGIN_CORE_NAMESPACE( Utils );


class CryptoHelpers
{
public:
    // For working with strings (returns null if conversion fails)
    static std::string encryptString(const std::string &instr,
                                     const std::string &passPhrase);
    static std::string decryptString(const std::string &instr,
                                                  const std::string &passPhrase);

    // For working with files
    static void encryptFile(const char *in, const char *out,
                            const char *passPhrase);
    static void decryptFile(const char *in, const char *out,
                                         const char *passPhrase);

    // Data Compression functions
    static std::string compress(const std::string &instr,
                                int level = DEFAULT_COMPRESSION_LEVEL);
    static std::string decompress(const std::string &instr);

    static int DEFAULT_COMPRESSION_LEVEL;
    static int MIN_COMPRESSION_LEVEL;
    static int MAX_COMPRESSION_LEVEL;

    // Different data encoding schemes
    static std::string toBase64(const std::string &instr);
    static std::string fromBase64(const std::string &instr);

    static std::string toBase16(const std::string &instr);
    static std::string fromBase16(const std::string &instr);

    // Random number generator
    static unsigned int rand();
    static std::string randData(int, int seed = -1);

};


GUTIL_END_CORE_NAMESPACE

#endif // CRYPTOHELPERS_H

#endif // GUTIL_ENCRYPTION
