/*Copyright 2010-2013 George Karagoulis

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
#include <new>

NAMESPACE_GUTIL

BaseException::BaseException(const char *name, const char *message, const char *file, int line)
    :m_message(0), What(name), File(file), Line(line)
{
    if(message){
        m_message = (char *)malloc(1 + strlen(message));
        strcpy(m_message, message);
    }
}

BaseException::BaseException(const BaseException &o)
    :m_message(0), What(o.What), File(o.File), Line(o.Line)
{
    if(o.m_message){
        m_message = (char *)malloc(1 + strlen(o.m_message));
        strcpy(m_message, o.m_message);
    }
}

BaseException &BaseException::operator = (const BaseException &o)
{
    this->~BaseException();
    new(this) BaseException(o);
    return *this;
}

BaseException::~BaseException()
{
    free(m_message);
}



Exception<false>::Exception(const char *file, int line, const char *name, const char *message)
    :BaseException(name == 0 ? "GUtil::Exception<false>" : name, message, file, line)
{}


END_NAMESPACE_GUTIL
