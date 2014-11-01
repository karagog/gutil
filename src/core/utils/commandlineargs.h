/*Copyright 2010-2015 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_COMMANDLINEARGS_H
#define GUTIL_COMMANDLINEARGS_H

#include <gutil/macros.h>
#include <gutil/string.h>

NAMESPACE_GUTIL;


/** Use this class as a wrapper for command line arguments.  You should dispose
    of this as soon as you're done using it, because it uses extra memory that
    is only useful while parsing the arguments.
*/

class CommandLineArgs :
        public Vector<String>
{
public:

    /** Just pass in argc and argv from the main function to initialize, and
        then you can access the arguments in the Vector, or use the convenience
        functions below.
    */
    CommandLineArgs(int argc, char **argv);

    /** Searches for a specific argument, with or without case-sensitivity.
        \returns -1 if not found
    */
    int FindArgument(const String &, bool case_sensitive = true) const;

    /** Returns whether the given argument is present in the argument list. */
    bool Contains(const String &arg, bool case_sensitive = true) const{
        return FindArgument(arg, case_sensitive) != -1;
    }

    /** Get the program name argument, or the element at argument 0 */
    String ProgramArgument() const{
        return 0 < Length() ? operator[](0) : "";
    }

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_COMMANDLINEARGS_H
