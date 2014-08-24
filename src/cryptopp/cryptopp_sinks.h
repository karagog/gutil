/*Copyright 2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_CRYPTOPP_SINKS_H
#define GUTIL_CRYPTOPP_SINKS_H

#include <cryptopp/simple.h>
#include "gutil_macros.h"

NAMESPACE_GUTIL;
class String;
class OutputInterface;
END_NAMESPACE_GUTIL;

NAMESPACE_GUTIL1(CryptoPP);


/** Used to adapt a generic OutputInterface into CryptoPP Sink. */
class OutputInterfaceSink : public ::CryptoPP::Sink
{
    OutputInterface &ref;
public:

    /** Just give it a string reference, and it will append all the data to it */
    OutputInterfaceSink(OutputInterface &s) :ref(s) {}
    virtual ~OutputInterfaceSink() {}

    virtual size_t Put2(const byte *, size_t, int, bool);
    virtual bool IsolatedFlush(bool, bool);

};


/** Used to adapt the GUtil String into CryptoPP Sink. */
class StringSink : public ::CryptoPP::Bufferless< ::CryptoPP::Sink>
{
    GUtil::String &ref;
public:

    /** Just give it a string reference, and it will append all the data to it */
    StringSink(GUtil::String &s) :ref(s) {}
    virtual ~StringSink() {}

    virtual size_t Put2(const byte *, size_t, int, bool);

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_CRYPTOPP_SINKS_H
