/*Copyright 2010 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef COMMANDLINEARGS_H
#define COMMANDLINEARGS_H

#include "gutil_macros.h"
#include <vector>
#include <string>

GUTIL_BEGIN_CORE_NAMESPACE(Utils);


// Use this class as a wrapper for command line arguments.  You should dispose
//  of this as soon as you're done using it, because it uses extra memory that
//  is only useful while parsing the arguments.

class CommandLineArgs
{
public:

    CommandLineArgs(int argc, char **argv);

    READONLY_PROPERTY(Arguments, std::vector<std::string>);

    int ArgCount() const;

    // Returns -1 if not found
    int FindArgument(const std::string &, bool case_sensitive = true);

    // Get the program name argument
    std::string ProgramArgument() const;

};


GUTIL_END_CORE_NAMESPACE

#endif // COMMANDLINEARGS_H
