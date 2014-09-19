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

#ifndef GUTIL_GLOBALLOGGER_H
#define GUTIL_GLOBALLOGGER_H

#include "gutil_exception.h"
#include "gutil_strings.h"
#include "gutil_map.h"
#include "gutil_smartpointer.h"
#include <exception>

NAMESPACE_GUTIL;


/** A static class that facilitates a global logging entity, accessible
    from anywhere inside your code.

    You can simply call GlobalLogger::LogMessage(...),
    and if there is a global logger set up then it will log,
    otherwise it will silently and efficiently ignore it.
*/
class GlobalLogger
{
public:

    /** Logs an exception. */
    static void LogException(const std::exception &ex){

    }

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_GLOBALLOGGER_H
