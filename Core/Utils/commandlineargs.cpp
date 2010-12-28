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

#include "commandlineargs.h"
#include "stringhelpers.h"
using namespace std;
GUTIL_USING_CORE_NAMESPACE(Utils);


CommandLineArgs::CommandLineArgs(int argc, char **argv)
    :_p_Arguments(argc)
{
    for(int i = 0; i < argc; i++)
        _p_Arguments[i] = argv[i];
}

int CommandLineArgs::FindArgument(const string &f, bool case_sensitive)
{
    int ret(-1);
    string search(f);

    if(!case_sensitive)
        search = StringHelpers::toUpper(search);

    vector<string> flags(_p_Arguments);
    for(int i(0); i < (int)flags.size(); i++)
    {
        string s(flags.at(i));

        if(!case_sensitive)
            s = StringHelpers::toUpper(s);

        if(s == search)
        {
            ret = i;
            break;
        }
    }

    return ret;
}

std::string CommandLineArgs::ProgramArgument() const
{
    if(_p_Arguments.size() > 0)
        return _p_Arguments[0];
    return "";
}
