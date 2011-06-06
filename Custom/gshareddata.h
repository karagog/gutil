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

#ifndef GSHAREDDATA_H
#define GSHAREDDATA_H

#include "gutil_macros.h"
#include <QReadWriteLock>
#include <QSharedData>

GUTIL_BEGIN_NAMESPACE( Custom );


// You can use this class instead of QSharedData, if you want to make use
//  of the virtual destructor
class GSharedData :
        public QSharedData
{
public:

    inline GSharedData(){}

    inline GSharedData(const GSharedData &o)
        :QSharedData(o){}

    // Allow derived classes to be deleted by this reference
    virtual ~GSharedData(){}

    // Use this lock to lock the shared data.  You must be careful
    //   to use locks carefully; the class provides a convenient lock that
    //   any objects sharing the data can use, but you have to verify your
    //   own locking mechanism.
    QReadWriteLock SharedLock;

    // This is so we can test if a template object is actually derived from GSharedData
    enum DerivedFromGSharedData{ IsDerivedFromGSharedData };

};


GUTIL_END_NAMESPACE

#endif // GSHAREDDATA_H
