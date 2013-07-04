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

#include "commandlineargs.h"
USING_NAMESPACE_GUTIL1(Utils);
USING_NAMESPACE_GUTIL1(DataObjects);

CommandLineArgs::CommandLineArgs(int argc, char **argv)
    :Vector<String>(argc)
{
    for(int i = 0; i < argc; i++)
        PushBack(argv[i]);
}

int CommandLineArgs::FindArgument(const String &f, bool case_sensitive) const
{
    int ret(-1);
    String search(case_sensitive ? f : f.ToUpper());

    const String *cur( ConstData() );
    for(GUINT32 i(0); i < Length(); ++i, ++cur)
    {
        String s(case_sensitive ? *cur : cur->ToUpper());

        if(s == search)
        {
            ret = i;
            break;
        }
    }

    return ret;
}
