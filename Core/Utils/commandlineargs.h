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

#ifndef COMMANDLINEARGS_H
#define COMMANDLINEARGS_H

#include "gutil.h"
#include "Core/DataObjects/gstring.h"

GUTIL_BEGIN_CORE_NAMESPACE(Utils);


// Use this class as a wrapper for command line arguments.  You should dispose
//  of this as soon as you're done using it, because it uses extra memory that
//  is only useful while parsing the arguments.

class CommandLineArgs :
        public DataObjects::Vector<DataObjects::String>
{
public:

    CommandLineArgs(int argc, char **argv);

    // Returns -1 if not found
    int FindArgument(const DataObjects::String &, bool case_sensitive = true) const;

    inline bool Contains(const DataObjects::String &arg, bool case_sensitive = true) const{
        return FindArgument(arg, case_sensitive) != -1;
    }

    // Get the program name argument
    inline DataObjects::String ProgramArgument() const{
        return Count() ? ConstData()[0] : "";
    }

};


GUTIL_END_CORE_NAMESPACE

#endif // COMMANDLINEARGS_H
