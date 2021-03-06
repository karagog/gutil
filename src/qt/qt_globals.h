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

#ifndef GUTIL_QT_GLOBALS_H
#define GUTIL_QT_GLOBALS_H

#include <gutil/string.h>
#include <gutil/exception.h>
#include <QMetaType>


Q_DECLARE_METATYPE( GUtil::String )
Q_DECLARE_METATYPE( GUtil::Exception<false> )
Q_DECLARE_METATYPE( GUtil::Exception<true> )


#endif // GUTIL_QT_GLOBALS_H
