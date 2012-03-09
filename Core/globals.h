/*Copyright 2010-2012 George Karagoulis

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

/** \file
    Global definitions that anyone can use.
*/

#include "gutil_macros.h"
#include "gutil_exception.h"
#include <malloc.h>


#if (defined(QT_DEBUG) || defined(DEBUG)) && !defined(GUTIL_DEBUG)
    /** Switch on debug features when building in debug mode. */
    #define GUTIL_DEBUG
#endif


#ifdef GUTIL_DEBUG
    #include <stdio.h>
    /** Special output method for debug mode. */
    #define GDEBUG(x)       std::cout << x << std::endl

    #define DEBUG_LOGGING
#else
    /** Special output method for debug mode. */
    #define GDEBUG(x)
#endif


/** The GUtil version.  This is always updated after every release.

    See DataObjects/version.h for a useful class for manipulating version strings,
    and for an explanation of the version system used by GUtil.
*/
#define GUTIL_VERSION       "0.0.0"



#if defined(GUTIL_DEBUG)
    #include <stdio.h>
    #include <stdlib.h>
    #define GASSERT(b)          if((!(b))){ printf("Assertion failed: (%s) on line %d of %s", #b, __LINE__, __FILE__); abort(); }
    #define GASSERT2(b, msg)    if((!(b))){ printf("Assertion failed: (%s) on line %d of %s: %s", #b, __LINE__, __FILE__, msg); abort(); }
#else
    /** Defines a debug assertion, which only executes in debug mode and aborts
        the program if the boolean condition returns false
    */
    #define GASSERT(b)

    /** Much like GASSERT, except you can also include a message when the program exits */
    #define GASSERT2(b, msg)
#endif




/** An internal struct for the foreach implementation */
template<class T, bool IsConst, bool Direction>struct GUtilForeachContainer{};

/** An internal struct specialization for the foreach implementation */
template<class T>struct GUtilForeachContainer<T, true, true>{
    inline GUtilForeachContainer(const T &container) :c(container), iter(c.begin()), end(c.end()) {}
    const T &c;
    typename T::const_iterator iter, end;
};

/** An internal struct specialization for the foreach implementation */
template<class T>struct GUtilForeachContainer<T, false, true>{
    inline GUtilForeachContainer(T &container) :c(container), iter(container.begin()), end(container.end()) {}
    T &c;
    typename T::iterator iter, end;
};

/** An internal struct specialization for the foreach implementation */
template<class T>struct GUtilForeachContainer<T, true, false>{
    inline GUtilForeachContainer(const T &container) :c(container), iter(c.rbegin()), end(c.rend()) {}
    const T &c;
    typename T::const_iterator iter, end;
};

/** An internal struct specialization for the foreach implementation */
template<class T>struct GUtilForeachContainer<T, false, false>{
    inline GUtilForeachContainer(T &container) :c(container), iter(container.rbegin()), end(container.rend()) {}
    T &c;
    typename T::iterator iter, end;
};


/** A syntactically concise iteration loop.

    This is better than Qt's foreach loop, because it doesn't make a copy of the
    container.  Use this version when you DON'T need to modify the container elements.

    \param var A variable declaration for the foreach body.  Use const-references whenever possible.
    \param container An STL-like container, which must implement an iterator class
            and begin()/end() functions
*/
#define G_FOREACH_CONST(var, container) \
    for(GUtilForeachContainer<__typeof__(container), true, true> cont(container); cont.iter != cont.end; ++cont.iter) \
        for(var = *cont.iter ;; __extension__({ break; }))

/** A syntactically concise iteration loop.

    This is better than Qt's foreach loop, because it doesn't make a copy of the
    container.  Use this version when you need to modify the container elements.

    \param var A variable declaration for the foreach body.  Use references whenever possible.
    \param container An STL-like container, which must implement an iterator class
            and begin()/end() functions
*/
#define G_FOREACH(var, container) \
    for(GUtilForeachContainer<__typeof__(container), false, true> cont(container); cont.iter != cont.end; ++cont.iter) \
        for(var = *cont.iter ;; __extension__({ break; }))


/** A syntactically concise iteration loop that starts at the back of the container.

    This is better than Qt's foreach loop, because it doesn't make a copy of the
    container.  Use this version when you DON'T need to modify the container elements.

    \param var A variable declaration for the foreach body.  Use const-references whenever possible.
    \param container An STL-like container, which must implement an iterator class
            and begin()/end() functions
*/
#define G_FOREACH_CONST_REVERSE(var, container) \
    for(GUtilForeachContainer<__typeof__(container), true, false> cont(container); cont.iter != cont.end; --cont.iter) \
        for(var = *cont.iter ;; __extension__({ break; }))

/** A syntactically concise iteration loop that starts at the back of the container.

    This is better than Qt's foreach loop, because it doesn't make a copy of the
    container.  Use this version when you need to modify the container elements.

    \param var A variable declaration for the foreach body.  Use references whenever possible.
    \param container An STL-like container, which must implement an iterator class
            and begin()/end() functions
*/
#define G_FOREACH_REVERSE(var, container) \
    for(GUtilForeachContainer<__typeof__(container), false, false> cont(container); cont.iter != cont.end; --cont.iter) \
        for(var = *cont.iter ;; __extension__({ break; }))


/** Assigns the source to destination, but only after checking that they are not equal

    \note Requires at least 2 instructions, comparison and assignment.  More instructions
    may come from the possibility that the destination type requires multiple instructions
    to evaluate its comparison/assignment.

    \warning Do not pass any "fancy" arguments into this macro.  For example,
    don't do SET_IF_NOT_EQUAL( tmp, var++ ), because it won't work as expected.  For
    efficiency's sake, no copies of the source/destination objects are made.
*/
#define SET_IF_NOT_EQUAL(destination, source) \
    if(destination != source) destination = source;


/** Allocates 1 element of type T.
    This will automatically throw a GUtil::BadAllocationException if the memory
    could not be allocated.
    \param T The type of object for which memory will be allocated
*/
#define GMALLOC( T )   GUtil::gmalloc<T>(1, __FILE__, __LINE__)

/** Allocates an array of N elements of type T.
    This will automatically throw a GUtil::BadAllocationException if the memory
    could not be allocated.
    \param T The type of object for which memory will be allocated
    \param N The number of objects
*/
#define GMALLOC2( T, N )   GUtil::gmalloc<T>(N, __FILE__, __LINE__)

/** Frees the memory allocated by GMALLOC */
#define GFREE( mem_ptr )   free(mem_ptr)



// In this section we define our potential COM exports (no namespaces when exporting)

#if !defined(GUTIL_COM_EXPORTS) && !defined(GUTIL_COM_IMPORTS)
NAMESPACE_GUTIL;
#endif


/** Returns the most significant set bit of a 8 bit number using a lookup table
    \note O(1).  This is implemented simply using a lookup table
*/
GUTIL_COM_EXTERN GUTIL_COM_DECLSPEC int FSB8(GUINT8 n);

/** Returns the most significant set bit of a 16 bit number in the minimum number of instructions
    using a lookup table.
    \note O(1).  This is computed in 3-4 cpu instructions using a lookup table
*/
GUTIL_COM_EXTERN GUTIL_COM_DECLSPEC int FSB16(GUINT16);

/** Returns the most significant set bit of a 32 bit number in the minimum number of instructions
    using a lookup table.
    \note O(1).  This is computed in 7 cpu instructions on average using a lookup table
*/
GUTIL_COM_EXTERN GUTIL_COM_DECLSPEC int FSB32(GUINT32);

/** Returns the most significant set bit of a 64 bit number in the minimum number of instructions
    using a lookup table.
    \note O(1).  This is computed in 9 cpu instructions on average using a lookup table
*/
GUTIL_COM_EXTERN GUTIL_COM_DECLSPEC int FSB64(GUINT64);


#if !defined(GUTIL_COM_EXPORTS) && !defined(GUTIL_COM_IMPORTS)
END_NAMESPACE_GUTIL
#endif



NAMESPACE_GUTIL;


/** The date and time at which the core library was built

    This string will also be exported with the COM interface, if that build
    flag is set
*/
extern const char *BUILD_TIME;


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


/** This template is used to provide consistent "null" values for
    various types.  The default template uses the type's default constructor
    as the null value, but you can specialize the template for your own
    classes to define a different null value.
*/
template<class T>class NullValue{ public: inline static T Value(){ return T(); } };

template<>class NullValue<GINT8>{ public: inline static GINT8 Value(){ return 0; } };
template<>class NullValue<GUINT8>{ public: inline static GUINT8 Value(){ return 0; } };
template<>class NullValue<GINT16>{ public: inline static GINT16 Value(){ return 0; } };
template<>class NullValue<GUINT16>{ public: inline static GUINT16 Value(){ return 0; } };
template<>class NullValue<GINT32>{ public: inline static GINT32 Value(){ return 0; } };
template<>class NullValue<GUINT32>{ public: inline static GUINT32 Value(){ return 0; } };
template<>class NullValue<GINT64>{ public: inline static GINT64 Value(){ return 0; } };
template<>class NullValue<GUINT64>{ public: inline static GUINT64 Value(){ return 0; } };
template<>class NullValue<GFLOAT32>{ public: inline static GFLOAT32 Value(){ return 0.0; } };
template<>class NullValue<GFLOAT64>{ public: inline static GFLOAT64 Value(){ return 0.0; } };
template<>class NullValue<GFLOAT96>{ public: inline static GFLOAT96 Value(){ return 0.0; } };

template<class T>class NullValue<T *>{ public: inline static T *Value(){ return 0; } };


// Here are some useful functions

/** A generic absolute value function. */
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

/** A compare function for floats.
    Returns -1 if f1 is less than f2, 0 if they're equal, 1 if the f2 is less than f1
*/
inline int gFuzzyCompare32(GFLOAT32 f1, GFLOAT32 f2){
    int ret;
    GFLOAT32 diff(f2 - f1);
    if(gAbs(diff) < 0.00001)
        ret = 0;
    else if(diff > 0)
        ret = -1;
    else
        ret = 1;
    return ret;
}

/** A compare function for floats.
    Returns -1 if f1 is less than f2, 0 if they're equal, 1 if the f2 is less than f1
*/
inline int gFuzzyCompare64(GFLOAT64 f1, GFLOAT64 f2){
    int ret;
    GFLOAT64 diff(f2 - f1);
    if(gAbs(diff) < 0.00000000001)
        ret = 0;
    else if(diff > 0)
        ret = -1;
    else
        ret = 1;
    return ret;
}

/** A compare function for floats.
    Returns -1 if f1 is less than f2, 0 if they're equal, 1 if the f2 is less than f1
*/
inline int gFuzzyCompare96(GFLOAT96 f1, GFLOAT96 f2){
    int ret;
    GFLOAT96 diff(f2 - f1);
    if(gAbs(diff) < 0.000000000000001)
        ret = 0;
    else if(diff > 0)
        ret = -1;
    else
        ret = 1;
    return ret;
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


/** This is an internal inline function.  Don't use this directly, instead
    use the macro GMALLOC.
*/
template<class T>
inline T *gmalloc(GUINT32 N, const char *file, GUINT32 line){
    T *ret( reinterpret_cast<T *>(malloc( N * sizeof(T) )) );
    if(!ret) throw GUtil::BadAllocationException<>(file, line);
    return ret;
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



END_NAMESPACE_GUTIL;




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



/** The suffix you find on the end of shared libraries in Windows */
#define GUTIL_LIBRARY_SUFFIX_WINDOWS ".dll"

/** The suffix you find on the end of shared libraries in Linux */
#define GUTIL_LIBRARY_SUFFIX_LINUX ".so.1"

#if defined(Q_OS_WIN)
#define GUTIL_LIBRARY_SUFFIX GUTIL_LIBRARY_SUFFIX_WINDOWS
#elif defined(Q_OS_LINUX)
#define GUTIL_LIBRARY_SUFFIX GUTIL_LIBRARY_SUFFIX_LINUX
#else
/** A platform-independent definition for the suffix of shared libraries */
#define GUTIL_LIBRARY_SUFFIX
#endif



#endif // GUTIL_GLOBALS_H
