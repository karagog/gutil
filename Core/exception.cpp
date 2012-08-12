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

#include "gutil_exception.h"
#include <cstring>
#include <malloc.h>

NAMESPACE_GUTIL

BaseException::BaseException(const char *name, const char *message, const char *file, int line)
    :m_message(0), What(name), File(file), Line(line)
{
    if(message)
    {
        unsigned int len( strlen(message) );
        m_message = reinterpret_cast<char *>(malloc(len + 1));
        memcpy(m_message, message, len);
        m_message[len] = '\0';
    }
}

BaseException::BaseException(const BaseException &o)
    :m_message(0), What(o.What), File(o.File), Line(o.Line)
{
    if(o.m_message)
    {
        unsigned int len( strlen(o.m_message) );
        m_message = reinterpret_cast<char *>(malloc(len + 1));
        memcpy(m_message, o.m_message, len);
        m_message[len] = '\0';
    }
}

BaseException &BaseException::operator = (const BaseException &o)
{
    What = o.What;
    File = o.File;
    Line = o.Line;
    if(m_message)
    {
        free(m_message);
        m_message = 0;
    }
    if(o.m_message)
    {
        unsigned int len( strlen(o.m_message) );
        m_message = reinterpret_cast<char *>(malloc(len + 1));
        memcpy(m_message, o.m_message, len);
        m_message[len] = '\0';
    }
    return *this;
}

BaseException::~BaseException()
{
    if(m_message)
        free(m_message);
}



#define EXCEPTION_STRING  "GUtil::Exception<false>"

Exception<false>::Exception()
    :BaseException(EXCEPTION_STRING)
{}

Exception<false>::Exception(const char *message, const char *name)
    :BaseException(name == 0 ? EXCEPTION_STRING : name, message)
{}

Exception<false>::Exception(const char *file, int line, const char *name, const char *message)
    :BaseException(name == 0 ? EXCEPTION_STRING : name, message, file, line)
{}


GUTIL_EXCEPTION_INSTANTIATE(Exception);
GUTIL_EXCEPTION_INSTANTIATE(NotImplementedException);
GUTIL_EXCEPTION_INSTANTIATE(BadAllocationException);
GUTIL_EXCEPTION_INSTANTIATE(ReadOnlyException);
GUTIL_EXCEPTION_INSTANTIATE(ArgumentException);
GUTIL_EXCEPTION_INSTANTIATE(ConversionException);
GUTIL_EXCEPTION_INSTANTIATE(DataTransportException);
GUTIL_EXCEPTION_INSTANTIATE(XmlException);
GUTIL_EXCEPTION_INSTANTIATE(EndOfFileException);
GUTIL_EXCEPTION_INSTANTIATE(LockException);
GUTIL_EXCEPTION_INSTANTIATE(NullReferenceException);
GUTIL_EXCEPTION_INSTANTIATE(IndexOutOfRangeException);
GUTIL_EXCEPTION_INSTANTIATE(ValidationException);
GUTIL_EXCEPTION_INSTANTIATE(InvalidCastException);
GUTIL_EXCEPTION_INSTANTIATE(NotFoundException);
GUTIL_EXCEPTION_INSTANTIATE(DivideByZeroException);
GUTIL_EXCEPTION_INSTANTIATE(UniqueKeyException);
GUTIL_EXCEPTION_INSTANTIATE(BuildException);
GUTIL_EXCEPTION_INSTANTIATE(InvalidStateTransitionException);


END_NAMESPACE_GUTIL
