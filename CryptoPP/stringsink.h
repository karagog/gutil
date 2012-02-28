/*Copyright 2010-2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_STRINGSINK_H
#define GUTIL_STRINGSINK_H

#include "gutil_strings.h"
#include "cryptopp-5.6.1/simple.h"

NAMESPACE_GUTIL1(CryptoPP);


/** Used to adapt my string into CryptoPP Sink. */
class StringSink :
        public ::CryptoPP::Bufferless< ::CryptoPP::Sink >
{
    GUtil::DataObjects::String &sref;
public:

    /** Just give it a string reference, and it will append all the data to it */
    inline StringSink(GUtil::DataObjects::String &s) :sref(s){}

    /** Overridden from ::CryptoPP::Bufferless */
    virtual size_t Put2(const byte *inString, size_t length, int messageEnd, bool blocking){
        sref.Append(reinterpret_cast<const char *>(inString), length);
        return 0;
    }

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_STRINGSINK_H
