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

#include "stringhelpers.h"
#include "strings_private.h"
#include "exception.h"
#include <cassert>
#include <map>

using namespace std;
using namespace GUtil;

static const char base64_digits[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char base64_padding = '=';

string StringHelpers::baseName(const string &s)
{
    //Note: if the filename has a trailing '/', this will return a null string
    string retstring(s.c_str());

    // If we can't parse the path correctly, return null
    if(!translate_path(retstring))
        return "";

    int lastSlash;

    //If there's no slash, basename is s
    if(static_cast<unsigned int>(lastSlash = retstring.find_last_of("/")) == string::npos)
        return retstring;

    return(retstring.substr(lastSlash + 1));
}

string StringHelpers::pathName(const string &s)
{
    //Note: if the filename has a trailing '/', this will return the full input string
    string retstring(s.c_str());

    // If we can't parse the path correctly, return null
    if(!translate_path(retstring))
        return "";

    int lastSlash;

    if(static_cast<unsigned int>(lastSlash = retstring.find_last_of("/")) == string::npos)
        return "./";		//If no slash was found, then the pwd is '.'

    return(retstring.substr(0, lastSlash + 1));
}

string StringHelpers::toBase16(const string &s)
{
    string res;
    for(int i = 0; i < (int)s.size(); i++)
    {
        char c = s[i];
        char lnib, rnib;

        lnib = (c & 0xf0) >> 4;
        rnib = c & 0x0f;

        res.append(1, hexToChar(lnib));
        res.append(1, hexToChar(rnib));
    }
    return res;
}

string StringHelpers::fromBase16(const string &s)
{
    if(s.size() % 2 != 0)
        throw GUtil::Exception("Input string was not in the correct format");

    string res;
    for(int i = 0; i < (int)s.size(); i += 2)
    {
        char c1 = s[i];
        char c2 = s[i + 1];
        char lnib, rnib;

        lnib = charToHex(c1) << 4;
        rnib = charToHex(c2);

        res.append(1, lnib | rnib);
    }
    return res;
}

string StringHelpers::toBase64(const string &instr)
{
    string outstr = "";

    char mem;
    char mask1;
    char mask2;
    const char orig_mask1 = 0b11111100;
    const char orig_mask2 = 0b00000011;
    int shift;
    int i;
    for(i = 0; i < (int)instr.length(); i++)
    {
        char o = instr.at(i);
        char six_bits;

        if(i % 3 == 0)
        {
            mask1 = orig_mask1;
            mask2 = orig_mask2;
            mem = 0x00;
            shift = 0;
        }
        else
        {
            mask1 = mask1 << 2;
            mask2 = (mask2 << 2) | (char)0x3;   // Fill new spaces with 1's
            shift += 2;
        }

        six_bits = ((unsigned char)((((unsigned char)(o & mask1)) >> shift) | (mem << (8 - shift)))) >> 2;
        mem = o & mask2;

        //assert((int)six_bits >= 0 && (int)six_bits < 64);
        outstr.append(&(base64_digits[(int)six_bits]), 1);

        // Take care of adding the 4th base64 character after every third ASCII character
        if(i % 3 == 2)
        {
            outstr.append(&(base64_digits[(int)mem]), 1);
        }
    }

    // Pad the end
    int r = i % 3;
    if(r != 0)
    {
        outstr.append(&(base64_digits[(int)(mem << (4 - shift))]), 1);
        while(r++ < 3)
        {
            outstr.append(&base64_padding, 1);
        }
    }

    return outstr;
}

string StringHelpers::fromBase64(const string &instr)
{
    string outstr = "";

    if(instr.length() % 4)
        throw new GUtil::Exception("Input string is not the right length");

    // Construct a map of the possible characters, which adds a bit of overhead to this function,
    //  but saves us a linear search of the characters for every base64 digit we find
    map<char, int> charmap;
    for(int i = 0; i < 64; i++)
        charmap[base64_digits[i]] = i;

    int len = instr.length();
    // Now translate the string
    for(int i = 0; i < len; i += 4)
    {
        // These chars are potentially padding, so we treat them specially for efficiency
        char tmp1 = instr.at(i);
        char tmp2 = instr.at(i + 1);
        char tmp3 = instr.at(i + 2);
        char tmp4 = instr.at(i + 3);

        // Check the characters for validation; don't accept any foreign characters
        map<char, int>::iterator it = charmap.find(tmp1);
        if(it == charmap.end())
            throw GUtil::Exception("Unrecognized base-64 character");

        it = charmap.find(tmp2);
        if(it == charmap.end())
            throw GUtil::Exception("Unrecognized base-64 character");

        unsigned char a = charmap[tmp1];
        unsigned char b = charmap[tmp2];
        unsigned char c = charmap[tmp3];
        unsigned char d = charmap[tmp4];

        char val[1] = { (a << 2) | (b >> 4) };
        outstr.append(val, 1);

        if(tmp3 != base64_padding)  // Equal signs are padding in base64
        {
            it = charmap.find(tmp3);
            if(it == charmap.end())
                throw GUtil::Exception("Unrecognized base-64 character");

            val[0] = (b << 4) | (c >> 2);
            outstr.append(val, 1);

            if(tmp4 != base64_padding)
            {
                it = charmap.find(tmp4);
                if(it == charmap.end())
                    throw GUtil::Exception("Unrecognized base-64 character");

                val[0] = (c << 6) | d;
                outstr.append(val, 1);
            }
            else if(i < (len - 4))
                throw GUtil::Exception("String is in unrecognized format");
        }
        else if(tmp4 != base64_padding)
            throw GUtil::Exception("String is in unrecognized format");
    }

    return outstr;
}

char StringHelpers::charToHex(char c)
{
    // Make it upper-case if it was lower
    if(c >= (char)0x61)
        c = c - 0x20;

    char lnib, rnib;
    char ret;

    lnib = (c & 0xf0) >> 4;
    rnib = c & 0x0f;

    if((lnib == 0x03) && (rnib < 0x0a))
    {	// If it's between 0 and 9
        ret = rnib;
    }
    else if((c >= (char)0x41) && (c <= (char)0x46))
    {	// if it's A-F
        ret = 0x09 + rnib;
    }
    else
        throw GUtil::Exception("Unrecognized hex character");

    return ret;
}

char StringHelpers::hexToChar(char c)
{
    char ret;

    // Make sure it's a valid hex digit
    if(0 != (c & 0xf0))
        throw GUtil::Exception("Unrecognized hex digit");

    // If it's less than 10, display a number
    else if(c < 0x0A)
        ret = 0x30 + c;

    // But if it's 10 or over, display a character A-F
    else
        ret = 0x41 + (c - 0x0A);

    return ret;
}

void StringHelpers::showBits(char c, ostream &str)
{
    unsigned char mask = 0b10000000;
    for(int i = 0; i < 8; i++)
    {
        str<<(mask & c ? "1" : "0");
        mask = mask >> 1;
    }
}

/*char StringHelpers::rotate(char c)
{
    char ret = 0x00;
    char mask = 0x01;
    int shift = 7;

    for(int i = 4; i > 0; i--)
    {
        unsigned char bit = c & mask;
        ret |= bit << shift;

        mask = mask << 1;
        shift -= 2;
    }

    shift = 1;
    for(int i = 4; i > 0; i--)
    {
        unsigned char bit = c & mask;
        ret |= bit >> shift;

        mask = mask << 1;
        shift += 2;
    }

    return ret;
}
*/
