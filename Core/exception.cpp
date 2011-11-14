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
using namespace GUtil::Core;

#define EXCEPTION_STRING  "GUtil::Core::Exception_base"

BaseException::BaseException()
    :What(EXCEPTION_STRING), File(0), Line(-1)
{}

BaseException::BaseException(const char *name)
    :What(name), File(0), Line(-1)
{}

BaseException::BaseException(const char *name, const char *file, int line)
    :What(name), File(file), Line(line)
{}

BaseException::BaseException(const char *file, int line)
    :What(EXCEPTION_STRING), File(file), Line(line)
{}
