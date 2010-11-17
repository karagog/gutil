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

#ifndef STRINGHELPERS_H
#define STRINGHELPERS_H

#include "gutil_macros.h"
#include <string>

GUTIL_BEGIN_CORE_NAMESPACE( Utils );


class StringHelpers
{
public:

    // Some functions for string manipulation
    static bool toInt(const std::string &, int &);
    static std::string toLower(const std::string &);
    static std::string toUpper(const std::string &);

    // Returns the name of the last element at the end of a file path
    //   Note: File slash directions are not guaranteed to be preserved!!
    static std::string baseName(const std::string &);

    // Returns the path of an absolute filename (always returns a trailing slash)
    static std::string pathName(const std::string &);

    // Base-64 conversions
    static std::string toBase64(const std::string &);
    static std::string fromBase64(const std::string &);

    // Hex conversions
    static std::string toBase16(const std::string &);
    static std::string fromBase16(const std::string &);

    // Manipulating hex strings
    static char charToHex(char);
    static char hexToChar(char);  // Always returns upper-case

    static void showBits(char c, std::ostream &);
};


GUTIL_END_CORE_NAMESPACE

#endif // STRINGHELPERS_H
