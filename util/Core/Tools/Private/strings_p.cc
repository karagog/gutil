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

#include "strings_p.h"
using namespace std;
using namespace GUtil::Core::Tools::Private;

bool translate_path(string &path)
{
    // Replace backslashes to forward slashes for compatibility

    unsigned int ind = path.find_first_of("\\");
    if(ind != string::npos)
    {
        // If there is a mix of forward slashes and backwards, I don't know how
        //  to handle it, so return false
        if(path.find_first_of("/") != string::npos)
            return false;

        do
        {
            path.replace(ind, 1, "/");
            ind = path.find_first_of("\\");
        }while(ind != string::npos);
    }

    // Strip the trailing slash so we get the right path name
    while(string::npos != (ind = path.find_last_of("/"))
        && ind == path.length() - 1)
        {
        path.resize(path.length() - 1);
    }

    return true;
}
