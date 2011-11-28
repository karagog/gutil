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

#include "exception.h"
#include <cstring>
#include <malloc.h>
using namespace GUtil::Core;

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
}

BaseException::~BaseException() throw(){
    if(m_message)
        free(m_message);
}
