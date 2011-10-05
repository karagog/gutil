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

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "gutil.h"
#include <string>

GUTIL_BEGIN_CORE_NAMESPACE(Utils);


class Environment
{
public:

    static std::string Username();

};


GUTIL_END_CORE_NAMESPACE;

#endif // ENVIRONMENT_H
