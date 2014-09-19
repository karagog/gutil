/*Copyright 2010-2014 George Karagoulis

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

NAMESPACE_GUTIL;

static void __allocate_and_copy_string(char const *source, char **target)
{
    *target = (char *)malloc(1 + strlen(source));
    strcpy(*target, source);
}


BaseException::BaseException(const char *message) noexcept
    :m_message(NULL)
{
    if(message) __allocate_and_copy_string(message, &m_message);
}

BaseException::BaseException(const BaseException &o) noexcept
    :m_message(NULL)
{
    if(o.m_message && 0 < strlen(o.m_message))
        __allocate_and_copy_string(o.m_message, &m_message);
}

BaseException &BaseException::operator = (const BaseException &o) noexcept
{
    free(m_message);
    m_message = NULL;
    if(o.m_message) 
        __allocate_and_copy_string(o.Message(), &m_message);
    return *this;
}

BaseException::~BaseException()
{
    free(m_message);
}


END_NAMESPACE_GUTIL;
