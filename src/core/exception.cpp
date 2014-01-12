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

static void __allocate_and_copy_string(char const *source, char **target)
{
    *target = (char *)malloc(1 + strlen(source));
    strcpy(*target, source);
}


BaseException::BaseException(const char *name, const char *message, const char *file, int line)
    :m_what(NULL), m_file(NULL), m_message(NULL), m_line(line)
{
    if(name)
        __allocate_and_copy_string(name, &m_what);
    if(file)
        __allocate_and_copy_string(file, &m_file);
    if(message)
        __allocate_and_copy_string(message, &m_message);
}

BaseException::BaseException(const BaseException &o)
    :m_what(NULL), m_file(NULL), m_message(NULL), m_line(o.Line())
{
    if(o.What())
        __allocate_and_copy_string(o.What(), &m_what);
    if(o.File())
        __allocate_and_copy_string(o.File(), &m_file);
    if(o.Message())
        __allocate_and_copy_string(o.Message(), &m_message);
}

void __setExceptionFileAndLineInfo(BaseException &ex, const char *file, int line)
{
    ex.m_line = line;
    if(file){
        free(ex.m_file);
        __allocate_and_copy_string(file, &ex.m_file);
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
    free(m_file);
    free(m_what);
}

const char *BaseException::What() const
{
    return m_what;
}

const char *BaseException::File() const
{
    return m_file;
}

int BaseException::Line() const
{
    return m_line;
}

const char *BaseException::Message() const
{
    return m_message;
}



Exception<false>::Exception(const char *file, int line, const char *name, const char *message)
    :BaseException(name == 0 ? "GUtil::Exception<false>" : name, message, file, line)
{}


END_NAMESPACE_GUTIL
