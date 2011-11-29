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

#include "gutil_macros.h"

#if (defined(QT_DEBUG) || defined(DEBUG)) && !defined(GUTIL_DEBUG)
    /** Switch on debug features when building in debug mode. */
    #define GUTIL_DEBUG
#endif


#ifdef GUTIL_DEBUG
    #include <iostream>
    /** Special output method for debug mode. */
    #define GDEBUG(x)       std::cout << x << std::endl
#else
    /** Special output method for debug mode. */
    #define GDEBUG(x)
#endif


/** The GUtil version.  This is always updated after every release.

    See DataObjects/version.h for a useful class for manipulating version strings,
    and for an explanation of the version system used by GUtil.
*/
#define GUTIL_VERSION       "0.0.0"




#ifndef GUTIL_COM_EXPORTS
namespace GUtil{
#endif


/** Use this template and its overrides to determine, at compile time,
    whether an item is a primitive type or more complex.

    You can create your own template specialization for your own classes
    to declare them primitive, thereby getting a boost in performance from
    some container classes, but by default any classes are assumed NOT primitive.
*/
template<class T>struct IsMovableType{ enum{ Value = 0 }; };

template<>struct IsMovableType<GINT8>{ enum{ Value = 1 }; };
template<>struct IsMovableType<GUINT8>{ enum{ Value = 1 }; };
template<>struct IsMovableType<GINT16>{ enum{ Value = 1 }; };
template<>struct IsMovableType<GUINT16>{ enum{ Value = 1 }; };
template<>struct IsMovableType<GINT32>{ enum{ Value = 1 }; };
template<>struct IsMovableType<GUINT32>{ enum{ Value = 1 }; };
template<>struct IsMovableType<GINT64>{ enum{ Value = 1 }; };
template<>struct IsMovableType<GUINT64>{ enum{ Value = 1 }; };
template<>struct IsMovableType<GFLOAT32>{ enum{ Value = 1 }; };
template<>struct IsMovableType<GFLOAT64>{ enum{ Value = 1 }; };
template<>struct IsMovableType<GFLOAT96>{ enum{ Value = 1 }; };

/** Any kind of pointer can be considered primitive. */
template<class T>struct IsMovableType<T *>{ enum{ Value = 1 }; };



// Here are some useful functions

template <class T>inline T gAbs(const T &v){ return v < 0 ? -v : v; }

/** Absolute value function which doesn't use branching.  This only works for ints */
template <>inline GINT8 gAbs<GINT8>(const GINT8 &v){
    const int mask( v >> (sizeof(GINT8) * 8 - 1) );
    return (v + mask) ^ mask;
}
/** Absolute value function which doesn't use branching.  This only works for ints */
template <>inline GINT16 gAbs<GINT16>(const GINT16 &v){
    const int mask( v >> (sizeof(GINT16) * 8 - 1) );
    return (v + mask) ^ mask;
}
/** Absolute value function which doesn't use branching.  This only works for ints */
template <>inline GINT32 gAbs<GINT32>(const GINT32 &v){
    const int mask( v >> (sizeof(GINT32) * 8 - 1) );
    return (v + mask) ^ mask;
}
/** Absolute value function which doesn't use branching.  This only works for ints */
template <>inline GINT64 gAbs<GINT64>(const GINT64 &v){
    const int mask( v >> (sizeof(GINT64) * 8 - 1) );
    return (v + mask) ^ mask;
}

/** Minimum value function.  Use as an alternate to qMin. */
template <class T> inline T gMin(const T &one, const T &two){
    return one < two ? one : two;
}

/** Maximum value function.  Use as an alternate to qMax(). */
template <class T> inline T gMax(const T &one, const T &two){
    return one < two ? two : one;
}


/** Declares a forever loop. */
#define G_FOREVER           for(;;)


/** Swap values in memory without using a temporary variable.
    \note This doesn't work when the two arguments share the same memory location.
    \note This evaluates to three processor instructions.
*/
#define GUTIL_SWAP(a, b)        (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

/** Swaps the values of locations of memory, without using a temporary variable.
    Use this version to copy by bytes; this is not efficient for large memory blocks.
    \sa gSwapWord32
*/
inline void gSwapByte(void *one, void *two, GINT32 size_in_bytes){
    GBYTE *b1(reinterpret_cast<GBYTE *>(one));
    GBYTE *b2(reinterpret_cast<GBYTE *>(two));
    while(--size_in_bytes >= 0){
        GUTIL_SWAP(*b1, *b2);
        ++b1, ++b2;
    }
}

/** Swaps the values of locations of memory, without using a temporary variable.
    Use this version to copy by 32-bit words, which is the most efficient if you have
    large memory blocks to swap.
    \sa gSwapByte, gSwapWord64
*/
inline void gSwapWord32(void *one, void *two, GINT32 size_in_ints){
    GUINT32 *b1(reinterpret_cast<GUINT32 *>(one));
    GUINT32 *b2(reinterpret_cast<GUINT32 *>(two));
    while(--size_in_ints >= 0){
        GUTIL_SWAP(*b1, *b2);
        ++b1, ++b2;
    }
}

/** Swaps the values of locations of memory, without using a temporary variable.
    Use this version to copy by 64-bit words, which is the most efficient if you have
    large memory blocks to swap.
    \sa gSwapByte, gSwapWord32
*/
inline void gSwapWord64(void *one, void *two, GINT64 size_in_ints){
    GUINT64 *b1(reinterpret_cast<GUINT64 *>(one));
    GUINT64 *b2(reinterpret_cast<GUINT64 *>(two));
    while(--size_in_ints >= 0){
        GUTIL_SWAP(*b1, *b2);
        ++b1, ++b2;
    }
}

/** Swaps the values of locations of memory, without using a temporary variable.
    Use this version to automatically run the function that is most appropriate,
    given whether the size is a multiple of four.
*/
inline void gSwap(void *one, void *two, GINT32 size_in_bytes)
{
    if(size_in_bytes & 0b0011)
        // If the size is not a multiple of 4
        gSwapByte(one, two, size_in_bytes);
    else
        gSwapWord32(one, two, size_in_bytes >> 2);
}


/** This is a lookup table which allows us to find the MSB in constant time (and very few instructions). */
extern "C" const char GUTIL_MSB_LOOKUP_TABLE[256];

/** Returns the most significant set bit of a 8 bit number using a lookup table
    \note O(1).  This is computed in 2 instructions (add and fetch), and is inlined
    to avoid the function call overhead
*/
inline int FSB8(GBYTE n){ return GUTIL_MSB_LOOKUP_TABLE[n]; }

/** Returns the most significant set bit of a 16 bit number in the minimum number of instructions
    using a lookup table.
    \note O(1).  This is computed in 3-4 cpu instructions
*/
#ifdef GUTIL_COM_EXPORTS
extern "C"
#endif
int FSB16(GUINT16);

/** Returns the most significant set bit of a 32 bit number in the minimum number of instructions
    using a lookup table.
    \note O(1).  This is computed in 7 cpu instructions on average.
*/
#ifdef GUTIL_COM_EXPORTS
extern "C"
#endif
int FSB32(GUINT32);

/** Returns the most significant set bit of a 64 bit number in the minimum number of instructions
    using a lookup table.
    \note O(1).  This is computed in 9 cpu instructions on average.
*/
#ifdef GUTIL_COM_EXPORTS
extern "C"
#endif
int FSB64(GUINT64);


/** Generates a 32-bit bitmask where all the bits up to index i are set to 1, starting from the least significant bit.
    \note Takes 2 instructions to generate.
*/
inline static GUINT32 GEN_BITMASK_32(int n)
{
    return (((GUINT32) 1) << (n + 1)) - 1;
}



/** Truncate the left n bits of the word. */
inline static GUINT32 TRUNCATE_LEFT_32(GUINT32 w, int n)
{
    // If we're a multiple of 32 then it doesn't actually shift us, so we return 0
    return (!n || (n & 31)) ?
                ((w << n) >> n) : 0;
}


/** Some global enumerations that you can use for consistency
    accross libraries and applications.  It's just for convention's sake.
 */
enum TimeFormatEnum
{
    Format12Hours = 0,
    Format24Hours = 1
};

/** Declares different sorting algorithms. */
enum SortTypeEnum
{
    MergeSort = 10
};



#ifndef GUTIL_COM_EXPORTS
} // namespace GUtil
#endif



#include "Core/DataObjects/flexibletypecomparer.h"

namespace GUtil
{

/** A convenience class that compares two objects with their less-than operator. */
template<class T>
class DefaultComparer : public Core::DataObjects::FlexibleTypeComparer<T>{};

}


#ifdef PI
#undef PI
#endif
/** The number PI, to 19 decimals. */
#define PI      3.1415926535897931160E+00


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
    not link with this library by undefining GUTIL_CRYPTOPP.


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
