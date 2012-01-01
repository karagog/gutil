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

#ifndef GUTIL_CORE_H
#define GUTIL_CORE_H

/** \file This file includes every class implemented in the core library.

    You can just #include this file and use the proper namespaces to get at the
    classes you need.  Then you'll have to link with GUtilCore

*/

#include "Core/exception.h"
#include "Core/extendedexception.h"
#include "Core/globals.h"

#include "Core/DataObjects/vector.h"
#include "Core/DataObjects/slist.h"
#include "Core/DataObjects/set.h"
#include "Core/DataObjects/map.h"
#include "Core/DataObjects/list.h"
#include "Core/DataObjects/interfaces.h"
#include "Core/DataObjects/heap.h"
#include "Core/DataObjects/gstring.h"
#include "Core/DataObjects/flags.h"
#include "Core/DataObjects/binarysearchtree.h"
#include "Core/DataObjects/dlist.h"
#include "Core/DataObjects/symmetricmatrix.h"

#include "Core/DataAccess/iointerface.h"
#include "Core/DataAccess/file.h"
#include "Core/DataAccess/console.h"
#include "Core/DataAccess/serialport.h"

#include "Core/Interfaces/iclonable.h"
#include "Core/Interfaces/icomparable.h"
#include "Core/Interfaces/iconvertable.h"
#include "Core/Interfaces/iequalitycomparer.h"
#include "Core/Interfaces/iequatable.h"
#include "Core/Interfaces/imatrix.h"
#include "Core/Interfaces/imemoryusagereporter.h"
#include "Core/Interfaces/ireadonlyobject.h"
#include "Core/Interfaces/iupdatable.h"
#include "Core/Interfaces/ivoidwrappers.h"
#include "Core/Interfaces/ixmlserializable.h"
#include "Core/Interfaces/icomparer.h"

#include "Core/Logging/abstractlogger.h"
#include "Core/Logging/filelogger.h"
#include "Core/Logging/consolelogger.h"
#include "Core/Logging/debuglogger.h"
#include "Core/Logging/globallogger.h"
#include "Core/Logging/grouplogger.h"

#include "Core/Utils/atomic.h"
#include "Core/Utils/commandlineargs.h"
#include "Core/Utils/environment.h"
#include "Core/Utils/linearsetalgebra.h"
#include "Core/Utils/smartpointer.h"
#include "Core/Utils/flexibletypecomparer.h"



#endif // GUTIL_CORE_H
