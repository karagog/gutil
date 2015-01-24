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

#ifndef GUTIL_GLOBALS_H
#define GUTIL_GLOBALS_H

/** \file
    Global definitions that anyone can use.
*/

#include <gutil/macros.h>
#include <gutil/exception.h>
#include <malloc.h>
#include <functional>

#if (defined(QT_DEBUG) || defined(DEBUG)) && !defined(GUTIL_DEBUG)
    #define GUTIL_DEBUG
#endif

#if defined(GUTIL_DEBUG3)
    #define GUTIL_DEBUG2
#endif

#if defined(GUTIL_DEBUG2)
    #define GUTIL_DEBUG
#endif


/** The GUtil version.  This is always updated after every release.

    See version.h for a useful class for manipulating version strings,
    and for an explanation of the version system used by GUtil.
*/
#define GUTIL_VERSION       "0.0.0"


/** Switch on debug features when building in debug mode. */
#ifdef GUTIL_DEBUG
    void __gutil_debug(const char *);
    void __gutil_assertion_failed(const char *assertion_text, const char *filename, int line_number);
    void __gutil_assertion_failed(const char *assertion_text, const char *filename, int line_number, const char *message);

    /** Special output method for debug mode. */
    #define GDEBUG(x)       __gutil_debug(x)

    /** Only outputs if the condition is true */
    #define GDEBUG2(c, x)       if(c) __gutil_debug(x)

    /** Defines a debug assertion, which only executes in debug mode and aborts
        the program if the boolean condition returns false
    */
    #define GASSERT(b)          if(!(b)){ __gutil_assertion_failed(#b, __FILE__, __LINE__); }

    /** Much like GASSERT, except you can also include a message when the program exits */
    #define GASSERT2(b, msg)    if(!(b)){ __gutil_assertion_failed(#b, __FILE__, __LINE__, msg); }

    #define DEBUG_LOGGING
#else
    #define GDEBUG(x)
    #define GDEBUG2(c, x)
    #define GASSERT(x)
    #define GASSERT2(x, y)
#endif



/** Loops N times.

    \param N The number of times to loop.  Interpreted as an unsigned integer.
*/
#define G_LOOP( N )       for(GUINT32 ___i(N); ___i != 0; --___i)


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
    \returns If any bits are set, it returns a number between 0 and 7.  If no bits are set
    it returns 255
*/
GUTIL_COM_EXTERN GUTIL_COM_DECLSPEC int FSB8(GUINT8 n);

/** Returns the most significant set bit of a 16 bit number in the minimum number of instructions
    using a lookup table.
    \note O(1).  This is computed in 3-4 cpu instructions using a lookup table
    \returns If any bits are set, it returns a number between 0 and 15.  If no bits are set
    it returns 255
*/
GUTIL_COM_EXTERN GUTIL_COM_DECLSPEC int FSB16(GUINT16);

/** Returns the most significant set bit of a 32 bit number in the minimum number of instructions
    using a lookup table.
    \note O(1).  This is computed in 7 cpu instructions on average using a lookup table
    \returns If any bits are set, it returns a number between 0 and 31.  If no bits are set
    it returns 255
*/
GUTIL_COM_EXTERN GUTIL_COM_DECLSPEC int FSB32(GUINT32);

/** Returns the most significant set bit of a 64 bit number in the minimum number of instructions
    using a lookup table.
    \note O(1).  This is computed in 9 cpu instructions on average using a lookup table
    \returns If any bits are set, it returns a number between 0 and 63.  If no bits are set
    it returns 255
*/
GUTIL_COM_EXTERN GUTIL_COM_DECLSPEC int FSB64(GUINT64);


#if !defined(GUTIL_COM_EXPORTS) && !defined(GUTIL_COM_IMPORTS)
END_NAMESPACE_GUTIL;
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
template<class T>class NullValue{ public: static T Value(){ return T(); } };

template<>class NullValue<GINT8>{ public: static GINT8 Value(){ return 0; } };
template<>class NullValue<GUINT8>{ public: static GUINT8 Value(){ return 0; } };
template<>class NullValue<GINT16>{ public: static GINT16 Value(){ return 0; } };
template<>class NullValue<GUINT16>{ public: static GUINT16 Value(){ return 0; } };
template<>class NullValue<GINT32>{ public: static GINT32 Value(){ return 0; } };
template<>class NullValue<GUINT32>{ public: static GUINT32 Value(){ return 0; } };
template<>class NullValue<GINT64>{ public: static GINT64 Value(){ return 0; } };
template<>class NullValue<GUINT64>{ public: static GUINT64 Value(){ return 0; } };
template<>class NullValue<GFLOAT32>{ public: static GFLOAT32 Value(){ return 0.0; } };
template<>class NullValue<GFLOAT64>{ public: static GFLOAT64 Value(){ return 0.0; } };
template<>class NullValue<GFLOAT96>{ public: static GFLOAT96 Value(){ return 0.0; } };

template<class T>class NullValue<T *>{ public: static T *Value(){ return 0; } };





// Here are some useful functions

/** A generic absolute value function. */
template <class T>static T Abs(const T &v){ return v < 0 ? -v : v; }

/** Absolute value function which doesn't use branching.  This only works for ints */
template <>GINT8 Abs<GINT8>(const GINT8 &v){
    const int mask( v >> (sizeof(GINT8) * 8 - 1) );
    return (v + mask) ^ mask;
}
/** Absolute value function which doesn't use branching.  This only works for ints */
template <>GINT16 Abs<GINT16>(const GINT16 &v){
    const int mask( v >> (sizeof(GINT16) * 8 - 1) );
    return (v + mask) ^ mask;
}
/** Absolute value function which doesn't use branching.  This only works for ints */
template <>GINT32 Abs<GINT32>(const GINT32 &v){
    const int mask( v >> (sizeof(GINT32) * 8 - 1) );
    return (v + mask) ^ mask;
}
/** Absolute value function which doesn't use branching.  This only works for ints */
template <>GINT64 Abs<GINT64>(const GINT64 &v){
    const int mask( v >> (sizeof(GINT64) * 8 - 1) );
    return (v + mask) ^ mask;
}

/** Compares two values to see if they are equal to within a
    specified tolerance.

    \param lhs The Left Hand Side of the comparison
    \param rhs The Right Hand Side of the comparison
    \param tolerance The tolerance within which the lhs and rhs are
    considered equal.

    \tparam T The type of the compared object must be comparable
    with 0 for this to work.   That means probably POD types like
    ints or floats, but you're not limited to those as long as you can
    compare if it's less than 0 for the Absolute Value function to work.

    \return 0 if the lhs and rhs are equal within the given tolerance.
    Otherwise it will be non-zero: -1 if lhs is less than rhs, and 1
    if lhs is greater than rhs.
*/
template<class T>int FuzzyCompare(const T &lhs, const T &rhs, const T &tolerance)
{
    int ret;
    if( Abs<T>(rhs - lhs) <= tolerance )
        ret = 0;
    else if(lhs < rhs)
        ret = -1;
    else
        ret = 1;
    return ret;
}

/** Minimum value function.  Use as an alternate to qMin. */
template <class T> static T Min(const T &one, const T &two){
    return one < two ? one : two;
}

/** Maximum value function.  Use as an alternate to qMax(). */
template <class T> static T Max(const T &one, const T &two){
    return one < two ? two : one;
}

/** Returns 1 if the number is Odd, or 0 if it's even. */
template<class T> static bool OddOrEven(T number){
    return number & 1;
}


/** Declares a forever loop that calls your function over and over
 *  until your function returns false.
 *  Your function must return a boolean: if you want to break, return false.
 *  If you want to continue to the next iteration, return true.
*/
template<class FUNCTION>
void Forever(FUNCTION f){
    for(;;) if(0 == f()) break;
}


template<bool can_break, class CONTAINER_TYPE, class FUNCTION>
struct foreach_t{
    foreach_t(const CONTAINER_TYPE &c, FUNCTION f){
        for(auto i(c.begin()), e(c.end()); i != e; ++i) f(*i);
    }
    foreach_t(CONTAINER_TYPE &c, FUNCTION f){
        for(auto i(c.begin()), e(c.end()); i != e; ++i) f(*i);
    }
};

template<class CONTAINER_TYPE, class FUNCTION>
struct foreach_t<true, CONTAINER_TYPE, FUNCTION>{
    foreach_t(const CONTAINER_TYPE &c, FUNCTION f){
        for(auto i(c.begin()), e(c.end()); i != e; ++i)
            if(0 == f(*i)) break;
    }
    foreach_t(CONTAINER_TYPE &c, FUNCTION f){
        for(auto i(c.begin()), e(c.end()); i != e; ++i)
            if(0 == f(*i)) break;
    }
};

template<bool can_break, class CONTAINER_TYPE, class FUNCTION>
struct foreach_reverse_t{
    foreach_reverse_t(const CONTAINER_TYPE &c, FUNCTION f){
        for(auto i(c.rbegin()), e(c.rend()); i != e; ++i) f(*i);
    }
    foreach_reverse_t(CONTAINER_TYPE &c, FUNCTION f){
        for(auto i(c.rbegin()), e(c.rend()); i != e; ++i) f(*i);
    }
};

template<class CONTAINER_TYPE, class FUNCTION>
struct foreach_reverse_t<true, CONTAINER_TYPE, FUNCTION>{
    foreach_reverse_t(const CONTAINER_TYPE &c, FUNCTION f){
        for(auto i(c.rbegin()), e(c.rend()); i != e; ++i)
            if(0 == f(*i)) break;
    }
    foreach_reverse_t(CONTAINER_TYPE &c, FUNCTION f){
        for(auto i(c.rbegin()), e(c.rend()); i != e; ++i)
            if(0 == f(*i)) break;
    }
};


/** Iterates through the container from beginning to end and applies
 *  the function to all objects. You are not allowed to modify any object.
 *
 *  \param c The container to iterate through. It must supply iterators via
 *              the standard begin() and end() functions.
 *  \param f The function or function object which is called on every object
 *              in the container. This can also be a lambda function. This function
 *              takes as a parameter the type of the object in the container, and
 *              returns true if the loop should continue, or false if you want to break.
 *  \param can_break Determines if you can break from the loop by returning false.
*/
template<bool can_break = false, class CONTAINER_TYPE, class FUNCTION>
void ForEach(const CONTAINER_TYPE &c, FUNCTION f){
    foreach_t<can_break, CONTAINER_TYPE, FUNCTION>(c, f);
}

/** Iterates through the container from beginning to end and applies
 *  the function to all objects. You can modify any object by declaring
 *  your function with a non-const reference parameter.
 *
 *  \param c The container to iterate through. It must supply iterators via
 *              the standard begin() and end() functions.
 *  \param f The function or function object which is called on every object
 *              in the container. This can also be a lambda function. This function
 *              takes as a parameter the type of the object in the container, and
 *              returns true if the loop should continue, or false if you want to break.
*/
template<bool can_break = false, class CONTAINER_TYPE, class FUNCTION>
void ForEach(CONTAINER_TYPE &c, FUNCTION f){
    foreach_t<can_break, CONTAINER_TYPE, FUNCTION>(c, f);
}

/** Iterates through the container from end to beginning and applies
 *  the function to all objects. You are not allowed to modify any object.
 *
 *  You can break out of the loop by returning false from your function.
 *
 *  \param c The container to iterate through. It must supply iterators via
 *              the standard rbegin() and rend() functions.
 *  \param f The function or function object which is called on every object
 *              in the container. This can also be a lambda function. This function
 *              takes as a parameter the type of the object in the container, and
 *              returns true if the loop should continue, or false if you want to break.
*/
template<bool can_break = false, class CONTAINER_TYPE, class FUNCTION>
void ForEachReverse(const CONTAINER_TYPE &c, FUNCTION f){
    foreach_reverse_t<can_break, CONTAINER_TYPE, FUNCTION>(c, f);
}

/** Iterates through the container from end to beginning and applies
 *  the function to all objects. You can modify any object by declaring
 *  your function with a non-const reference parameter.
 *
 *  You can break out of the loop by returning false from your function.
 *
 *  \param c The container to iterate through. It must supply iterators via
 *              the standard rbegin() and rend() functions.
 *  \param f The function or function object which is called on every object
 *              in the container. This can also be a lambda function. This function
 *              takes as a parameter the type of the object in the container, and
 *              returns true if the loop should continue, or false if you want to break.
*/
template<bool can_break = false, class CONTAINER_TYPE, class FUNCTION>
void ForEachReverse(CONTAINER_TYPE &c, FUNCTION f){
    foreach_reverse_t<can_break, CONTAINER_TYPE, FUNCTION>(c, f);
}


/** A nifty try catch finally construction made possible by c++11's new lambda functions.
 *  It behaves just like you would want a try-catch-finally to behave, with a few minor
 *  querks. Firstly you can only catch one type of exception, the base type
 *  of all your exceptions, and if you want to know its type you dynamic cast its pointer.
 *
 *  The default usage, which catches std::exception and has no code looks like this:
 *
 *  TryFinally(
 *      []{},                           // Try body
 *      [](const std::exception &){},   // Catch body
 *      []{});                          // Finally body
 *
 *  Note that the catch function should handle with a const reference, because it
 *  allows you to check which type the exception instance is.
 *
 *  If you want to catch a custom exception type other than std::exception, call it "MyException",
 *  it looks like this:
 *
 *  TryFinally<MyException>(
 *      []{},                       // Try body
 *      [](const MyException &){},  // Catch body
 *      []{});                      // Finally body
 *
 *  \tparam EXCEPTION The type of exception which will be caught. If an exception is thrown that
 *              doesn't derive from this base type, the finally body will be executed but not the catch.
 *  \param _try The try body. This is executed first. Exceptions thrown here will be handled by the catch body.
 *  \param _catch The catch body. This is only executed if an exception was thrown inside the try block.
 *          It is required to take the handled exception class as an argument.
 *  \param _finally The finally body. This is executed regardless of whether an exception was thrown,
 *              either from the try body or the catch body.
*/
template<class EXCEPTION = std::exception, class TRY_BLOCK, class CATCH_BLOCK, class FINALLY_BLOCK>
inline void TryFinally(TRY_BLOCK _try, CATCH_BLOCK _catch, FINALLY_BLOCK _finally)
{
    try{ _try(); }
    catch(EXCEPTION &e) {
        try{ _catch(e); }
        catch(...) { _finally(); throw; }
    }
    catch(...){ _finally(); throw; }
    _finally();
}


/** A helper class for the finally() macro. Do not use directly; use finally() instead. */
class finally_t{
    GUTIL_DISABLE_COPY(finally_t);
    std::function<void(void)> _finally;
public:
    template<class FUNCTION>inline finally_t(FUNCTION f) :_finally{f} {}
    inline ~finally_t(){ _finally(); }
};

#ifdef finally
#undef finally
#endif // finally

/** A macro that executes the function even if flow is broken due to an exception.
    You can only use this once per scope, but they can shadow each other
    inside nested scopes.

    Use it like this:

    void risky_function()
    {
        finally([]{
            // Put cleanup code here, before the risky stuff
        });

        // Do risky stuff that might throw an exception
    }
*/
#define finally(F)  GUtil::finally_t ___gutil_finally(F)



/** Swap values in memory without using a temporary variable.  This only works
    if the data is binary-movable.

    \note This doesn't work when the two arguments share the same memory location.
    \note This evaluates to three processor instructions.
*/
#define GUTIL_SWAP_INPLACE(a, b)        (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

/** Swaps the values of locations of memory, without using a temporary variable.
    Use this version to copy by bytes; this is not efficient for large memory blocks.
    \sa gSwapWord32
*/
static inline void gSwapByte(void *one, void *two, GINT32 size_in_bytes){
    GBYTE *b1(reinterpret_cast<GBYTE *>(one));
    GBYTE *b2(reinterpret_cast<GBYTE *>(two));
    while(--size_in_bytes >= 0){
        GUTIL_SWAP_INPLACE(*b1, *b2);
        ++b1, ++b2;
    }
}

/** Swaps the values of locations of memory, without using a temporary variable.
    Use this version to copy by 32-bit words, which is the most efficient if you have
    large memory blocks to swap.
    \sa gSwapByte, gSwapWord64
*/
static inline void gSwapWord32(void *one, void *two, GINT32 size_in_ints){
    GUINT32 *b1(reinterpret_cast<GUINT32 *>(one));
    GUINT32 *b2(reinterpret_cast<GUINT32 *>(two));
    while(--size_in_ints >= 0){
        GUTIL_SWAP_INPLACE(*b1, *b2);
        ++b1, ++b2;
    }
}

/** Swaps the values of locations of memory, without using a temporary variable.
    Use this version to copy by 64-bit words, which is the most efficient if you have
    large memory blocks to swap.
    \sa gSwapByte, gSwapWord32
*/
static inline void gSwapWord64(void *one, void *two, GINT64 size_in_ints){
    GUINT64 *b1(reinterpret_cast<GUINT64 *>(one));
    GUINT64 *b2(reinterpret_cast<GUINT64 *>(two));
    while(--size_in_ints >= 0){
        GUTIL_SWAP_INPLACE(*b1, *b2);
        ++b1, ++b2;
    }
}

/** Swaps the values of locations of memory, without using a temporary variable.
    Use this version to automatically run the function that is most appropriate,
    given whether the size is a multiple of four.
*/
static inline void gSwap(void *one, void *two, GINT32 size_in_bytes)
{
    if(size_in_bytes & 0b0011)
        // If the size is not a multiple of 4
        gSwapByte(one, two, size_in_bytes);
    else
        gSwapWord32(one, two, size_in_bytes >> 2);
}


/** A templated version of gSwap with a more "traditional" implementation
    that uses a temp variable to copy.
*/
template<class T>
static inline void gSwap(T *one, T *two)
{
    T bkp(*one);
    *one = *two;
    *two = bkp;
}



/** Generates a 32-bit bitmask where all the bits up to index i are set to 1, starting from the least significant bit.
    \note Takes 2 instructions to generate.
*/
template<class INT_TYPE>
static inline INT_TYPE GEN_BITMASK(int n)
{
    return (((INT_TYPE) 0x1) << (n + 1)) - 1;
}


/** Truncate the left n bits of the word. */
static inline GUINT32 TRUNCATE_LEFT_32(GUINT32 w, int n)
{
    // If we're a multiple of 32 then it doesn't actually shift us, so we return 0
    return (!n || (n & 31)) ?
                ((w << n) >> n) : 0;
}


/** This is an internal function.  Don't use this directly, instead
    use the macro GMALLOC.
*/
template<class T>
static T *gmalloc(GUINT32 N, const char *file, GUINT32 line){
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


/** Returns a full word of the given type with all bits initialized to init_val */
template<class INT_TYPE> static INT_TYPE BitMask(bool init_val){
    return init_val ? ((INT_TYPE)-1) : ((INT_TYPE)0);
}


END_NAMESPACE_GUTIL;




#ifdef PI
#undef PI
#endif
/** The number PI, approximately */
#define PI      ((GFLOAT64)3.14159265358979311600e+00)


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
#define GUTIL_SHAREDLIBRARY_SUFFIX_WINDOWS  ".dll"
#define GUTIL_SHAREDLIBRARY_SUFFIX_LINUX    ".so"


#if defined(__WIN32)
#define GUTIL_SHAREDLIBRARY_SUFFIX  GUTIL_SHAREDLIBRARY_SUFFIX_WINDOWS
#elif defined(__unix__)
#define GUTIL_SHAREDLIBRARY_SUFFIX  GUTIL_SHAREDLIBRARY_SUFFIX_LINUX
#else
/** A platform-independent definition for the suffix of shared libraries */
#define GUTIL_SHAREDLIBRARY_SUFFIX
#endif


#endif // GUTIL_GLOBALS_H
