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

/** \file Global definitions that anyone can use. */

#if (defined(QT_DEBUG) || defined(DEBUG)) && !defined(GUTIL_DEBUG)
    /** Switch on debug features when building in debug mode. */
    #define GUTIL_DEBUG
#endif


#ifdef GUTIL_DEBUG
    #include <iostream>
    /** Special output method for debug mode. */
    #define GDEBUG(x)       std::cout << x
#else
    /** Special output method for debug mode. */
    #define GDEBUG(x)
#endif


/** The GUtil version.  This is always updated after every release.

    See DataObjects/version.h for a useful class for manipulating version strings,
    and for an explanation of the version system used by GUtil.
*/
#define GUTIL_VERSION       "0.0.0"


/** An 8 bit integer*/
#define GINT8   char

/** An 8 bit unsigned integer*/
#define GUINT8   unsigned char

/** A 16 bit integer. */
#define GINT16  short

/** A 16 bit unsigned integer. */
#define GUINT16  unsigned short

/** A 32 bit integer. */
#define GINT32  int

/** A 32 bit unsigned integer. */
#define GUINT32 unsigned int

/** A 64 bit integer. */
#define GINT64  long long

/** A 64 bit unsigned integer. */
#define GUINT64 unsigned long long




// Here are some useful functions

/** Absolute value function.  Use as an alternate to qAbs(). */
template <class T> inline T gAbs(const T &v){ return v < 0 ? -v : v; }

/** Minimum value function.  Use as an alternate to qMin. */
template <class T> inline T gMin(const T &one, const T &two){
    return one < two ? one : two;
}

/** Maximum value function.  Use as an alternate to qMax(). */
template <class T> inline T gMax(const T &one, const T &two){
    return one < two ? two : one;
}



/** Returns the most significant set bit of a 32 bit number in the minimum number of instructions.
    \note O(1).  This is computed in 7 cpu instructions on average.
*/
extern "C" int MSB32(GUINT32);

/** Returns the most significant set bit of a 64 bit number in the minimum number of instructions.
    \note O(1).  This is computed in 9 cpu instructions on average.
*/
extern "C" int MSB64(GUINT64);

/** This is a lookup table which allows us to find the MSB in constant time (and very few instructions). */
extern const char MSB_LOOKUP_TABLE[256];


/** Generates a 32-bit bitmask where the first n bits are set to 1.
    \note O(1), takes 4 instructions to generate.
*/
inline static GUINT32 GEN_BITMASK_32(int n)
{
    if(--n < 0) return 0;
    GINT32 ret(0x80000000);
    return static_cast<GUINT32>(ret >> n);
}



/** Truncate the left n bits of the word. */
inline static GUINT32 &TRUNCATE_LEFT_32(GUINT32 &w, int n)
{
    return w = ((w << n) >> n);
}


/** Some global enumerations that you can use for consistency
    accross libraries and applications.  It's just for convention's sake.
 */
enum GUtilTimeFormatEnum
{
    GUtilFormat12Hours = 0,
    GUtilFormat24Hours = 1
};


// Some useful time constants

/** The number of seconds in a minute. */
#define MINUTE_IN_SECONDS       60

/** The number of seconds in an hour. */
#define HOUR_IN_SECONDS         60 * 60

/** The number of seconds in a day. */
#define DAY_IN_SECONDS          60 * 60 * 24

/** The number of seconds in the shortest month; February in a non-leap year. */
#define MONTH_IN_SECONDS_LB     DAY_IN_SECONDS * 28

/** The number of seconds in the longest possible month */
#define MONTH_IN_SECONDS_UB     DAY_IN_SECONDS * 31

/** The number of seconds in a normal year. */
#define YEAR_IN_SECONDS_LB      DAY_IN_SECONDS * 365

/** The number of seconds in a normal year. */
#define YEAR_IN_SECONDS         YEAR_IN_SECONDS_LB

/** The number of seconds in a leap year. */
#define YEAR_IN_SECONDS_UB      DAY_IN_SECONDS * 366



// We'll put the namespace document here, because it's as good a place as any.
/**

    \namespace GUtil Encapsulates George's Utility Library.

    Any time you make use of GUtil you will be using something in this namespace.

    See the documentation for any of the namespaces for a deeper explanation of
    the types of classes or code they contain.

    Anything not in the Core namespace may be built off of the Qt library.


    \namespace GUtil::Core A subset of libraries that aren't dependent on Qt.

    There will be no link dependency on Qt, so you can use this library in applications
    where Qt is not available or desireable.  This is where the encryption functions lie,
    which are dependent on Crypto++, a third-party encryption library.  You can optionally
    not link with this library by undefining GUTIL_ENCRYPTION.


    \namespace GUtil::ThirdParty A separate dll to contain code I didn't write.

    Yeah I'm a moocher, but I'm giving enough back to pay it forward.


    \namespace GUtil::Interfaces Interface classes for consistency/convention.

    Some of these classes are not strictly interfaces, in that they do have some functions
    in addition to the pure virtual functions that consumers of the interface need to implement.
    These extra functions provide more functionality to the interfaces, and as a goal they are
    kept as thin as possible.


    \namespace GUtil::Utils Utility functions and classes.

    A whole array of different useful functions and classes.  You can use build switches
    in the .pro file to adjust Qt dependencies.


    \namespace GUtil::Custom Classes that are derived from non-GUI Qt classes.

    They provide extra functionality that I want and the Qt team won't implement/haven't
    yet implemented.  It should only require the QtCore library, otherwise it belongs in
    GUICustom.


    \namespace GUtil::UI User Interface widgets


    \namespace GUtil::DataObjects Classes relating to data representation.


    \namespace GUtil::DataModels Data model classes that work with Qt's model-view framework.


    \namespace GUtil::ModelViews Model view classes that work with Qt's model-view framework.


    \namespace GUtil::DataAccess Classes relating to data transfer.


    \namespace GUtil::Logging Useful logging framework that uses Qt's signal/slot mechanism.


    \namespace GUtil::BusinessObjects Higher-level classes built atop the other libraries,
    which provide advanced functionality.


    \namespace GUtil::Controls UI controls built with Qt's widget system.


    \namespace GUtil::Images A library to contain common icons and image resources.

    Nothing is built into this library by default, you must add the resources to the .qrc file
    if you want to use them, that way you optimize the dll size and the GUtil build speed.

    You must use the Q_INIT_RESOURCES() macro when you want to use this library, with the
    argument being the qrc file name, without the .qrc appended.


    \namespace GUtil::GUICustom Classes derived from Qt's GUI framework, which provide extra functionality.


    \namespace GUtil::Test Classes useful for testing applications and implementations.

    It links with Qt's Test library.  Normal applications don't need this library, it is just
    for testing situations.

*/

#endif // GUTIL_GLOBALS_H