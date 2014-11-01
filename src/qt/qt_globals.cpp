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

/** \file This file defines any global/static variables used in the library,
 *  as well as any initialization code.
*/

#include "qt_globals.h"

NAMESPACE_GUTIL1(Qt);


static void __initialize_qt_metatypes()
{
    qRegisterMetaType< GUtil::Exception<false> >("GUtil::Exception<false>");
    qRegisterMetaType< GUtil::Exception<true> >("GUtil::Exception<true>");
    qRegisterMetaType< GUtil::String >("GUtil::String");
}


class LOADER
{
public:
    LOADER()
    {
        GDEBUG("Initializing GUtil Qt Library...");
        __initialize_qt_metatypes();
    }

    ~LOADER()
    {
        GDEBUG("Uninitializing GUtil Qt Library...");
    }
};

static LOADER __ldr;


END_NAMESPACE_GUTIL1;
