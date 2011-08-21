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

#ifndef GUTIL_GLOBALS_H
#define GUTIL_GLOBALS_H

// For convenience, we'll also include gutil macros
#include "gutil_macros.h"


// Here you can find the GUtil version; see DataObjects/version.h
#define GUTIL_VERSION       "0.0.0"


// Here are some useful functions

template <class T> inline T gAbs(const T &v){ return v < 0 ? -v : v; }

template <class T> inline T gMin(const T &one, const T &two){
    return one < two ? one : two;
}
template <class T> inline T gMax(const T &one, const T &two){
    return one < two ? two : one;
}


// Some global enumerations that you can use to be consistent across libraries
enum GUtilTimeFormatEnum
{
    GUtilFormat12Hours = 0,
    GUtilFormat24Hours = 1
};


// Some useful time constants
#define MINUTE_IN_SECONDS       60
#define HOUR_IN_SECONDS         60 * 60
#define DAY_IN_SECONDS          60 * 60 * 24

#define MONTH_IN_SECONDS_LB     DAY_IN_SECONDS * 28
#define MONTH_IN_SECONDS_UB     DAY_IN_SECONDS * 31

#define YEAR_IN_SECONDS_LB      DAY_IN_SECONDS * 365
#define YEAR_IN_SECONDS         YEAR_IN_SECONDS_LB
#define YEAR_IN_SECONDS_UB      DAY_IN_SECONDS * 366


#endif // GUTIL_GLOBALS_H
