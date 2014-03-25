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

#ifdef _WIN32
#include <windows.h>
#include <lmcons.h>
#else
#include <stdlib.h>
#endif

#include "environment.h"

NAMESPACE_GUTIL;


String Environment::Username()
{
    String ret;

#ifdef _WIN32

//#if defined(UNICODE)
//    if ( qWinVersion() & Qt::WV_NT_based )
//    {
//        TCHAR winUserName[UNLEN + 1]; // UNLEN is defined in LMCONS.H
//        DWORD winUserNameSize = sizeof(winUserName);
//        GetUserName( winUserName, &winUserNameSize );
//        ret = qt_winQString( winUserName );
//    } else
//#endif

    {
        char winUserName[UNLEN + 1]; // UNLEN is defined in LMCONS.H
        DWORD winUserNameSize = sizeof(winUserName);
        GetUserNameA( winUserName, &winUserNameSize );
        ret = winUserName;
    }
#else

    ret = getenv("USER");

#endif

    return ret;
}


END_NAMESPACE_GUTIL;
