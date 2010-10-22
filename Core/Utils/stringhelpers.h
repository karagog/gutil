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

#include <string>
using namespace std;

namespace GUtil
{
    namespace Core
    {
        namespace Utils
        {
            class StringHelpers
            {
            public:

                // Some functions for string manipulation
                static bool toInt(const string &, int &);
                static string toLower(const string &);
                static string toUpper(const string &);

                // Returns the name of the last element at the end of a file path
                //   Note: File slash directions are not guaranteed to be preserved!!
                static string baseName(const string &);

                // Returns the path of an absolute filename (always returns a trailing slash)
                static string pathName(const string &);

                // Base-64 conversions
                static string toBase64(const string &);
                static string fromBase64(const string &);

                // Hex conversions
                static string toBase16(const string &);
                static string fromBase16(const string &);

                // Manipulating hex strings
                static char charToHex(char);
                static char hexToChar(char);  // Always returns upper-case

                static void showBits(char c, ostream &);
            };
        }
    }
}

#endif // STRINGHELPERS_H
