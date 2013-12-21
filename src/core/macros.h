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

#ifndef GUTIL_MACROS_H
#define GUTIL_MACROS_H

/** \file

    Declares useful global macros used by many classes inside GUtil.  Often if you include
    those classes you are automatically including gutil.h, but it is useful even on its own.
*/


#include <limits.h>


/** Represents a generic 8 bits of memory. */
#define GBYTE unsigned char

/** An 8 bit integer*/
#define GINT8       char
/** The max value of a GINT8 */
#define GINT8_MAX     ((GINT8)0x7F)
/** The min value of a GINT8 */
#define GINT8_MIN     ((GINT8)0x80)

/** An 8 bit unsigned integer*/
#define GUINT8      unsigned char
/** The max value of a GUINT8 */
#define GUINT8_MAX     ((GUINT8)0xFF)

/** A 16 bit integer. */
#define GINT16      short
/** The max value of a GINT16 */
#define GINT16_MAX     ((GINT16)0x7FFF)
/** The min value of a GINT16 */
#define GINT16_MIN     ((GINT16)0x8000)

/** A 16 bit unsigned integer. */
#define GUINT16     unsigned short
/** The max value of a GUINT16 */
#define GUINT16_MAX     ((GUINT16)0xFFFF)

/** A 32 bit integer. */
#define GINT32      int
/** The max value of a GINT32 */
#define GINT32_MAX      ((GINT32)0x7FFFFFFF)
/** The min value of a GINT32 */
#define GINT32_MIN      ((GINT32)0X80000000)

/** A 32 bit unsigned integer. */
#define GUINT32     unsigned int
/** The max value of a GUINT32 */
#define GUINT32_MAX     ((GUINT32)0xFFFFFFFF)

/** A 64 bit integer. */
#define GINT64      long long
/** The max value of a GINT64 */
#define GINT64_MAX      (0x7FFFFFFFFFFFFFFFLL)
/** The min value of a GINT64 */
#define GINT64_MIN      (0x8000000000000000LL)

/** A 64 bit unsigned integer. */
#define GUINT64     unsigned long long
/** The max value of a GUINT64 */
#define GUINT64_MAX     (0xFFFFFFFFFFFFFFFFLL)

/** A 32-bit floating point number. */
#define GFLOAT32    float

/** A 64-bit floating point number. */
#define GFLOAT64    double

/** A 96-bit floating point number. */
#define GFLOAT96    long double



/** Use this to succinctly declare property accessors.

   It declares a Get and Set method for an object of the provided variable type and name,
   and also a private variable to hold the value.
*/
#define PROPERTY( name, type ) \
private: \
    type _p_##name; \
public: \
    const type &Get##name() const{ return _p_##name; } \
    void Set##name(const type &value){ _p_##name = value; }


/** Use this to succinctly declare property accessors to a pointer.

    It declares a Get and Set method for a pointer to an object of
    the provided variable type and name, and also a private variable
    to hold the value.
*/
#define PROPERTY_POINTER( name, ptr_type ) \
private: \
    ptr_type *_p_##name; \
public: \
    ptr_type *Get##name() const{ return _p_##name; } \
    void Set##name(ptr_type *value){ _p_##name = value; }


/** Use this to succinctly declare protected property accessors.

   It declares a Get and Set method for an object of the provided variable type and name,
   and also a private variable to hold the value.
*/
#define PROTECTED_PROPERTY( name, type ) \
private: \
    type _p_##name; \
protected: \
    const type &Get##name() const{ return _p_##name; } \
    void Set##name(const type &value){ _p_##name = value; }


/** Use this to succinctly declare a readonly property accessor.

   It declares a Get method for an object of the provided variable type and name,
   but does not give a Set method with which to change the value.
*/
#define READONLY_PROPERTY( name, type ) \
private: \
    type _p_##name; \
public: \
    const type &Get##name() const{ return _p_##name; }


/** Use this to succinctly declare a readonly property accessor to a pointer.

    It declares a Get method for a pointer to an object of
    the provided variable type and name, and also a private variable
    to hold the value.
*/
#define READONLY_PROPERTY_POINTER( name, type ) \
private: \
    type *_p_##name; \
public: \
    type *Get##name() const{ return _p_##name; }


/** Use this to succinctly declare a readonly protected property accessor.

   It declares a Get method for an object of the provided variable type and name,
   and also a private variable to hold the value.
*/
#define PROTECTED_READONLY_PROPERTY( name, type ) \
private: \
    type _p_##name; \
protected: \
    const type &Get##name() const{ return _p_##name; }





#ifndef NO_GUTIL_NAMESPACES

/** A macro to begin a nested GUtil namespace. */
#define NAMESPACE_GUTIL2( n1, n2 )      namespace GUtil{ namespace n1{ namespace n2{
/** A macro to begin a GUtil sub-namespace. */
#define NAMESPACE_GUTIL1( n )           namespace GUtil{ namespace n {
/** A macro to begin a gutil namespace. */
#define NAMESPACE_GUTIL                 namespace GUtil{


/** Ends a 2-deep nested GUtil namespace. */
#define END_NAMESPACE_GUTIL2            }}}
/** Ends a GUtil sub-namespace. */
#define END_NAMESPACE_GUTIL1            }}
/** Ends a GUtil namespace declaration. */
#define END_NAMESPACE_GUTIL             }

/** Use the provided nested GUtil namespace */
#define USING_NAMESPACE_GUTIL2( n1, n2 )        using namespace GUtil::n1::n2
/** Use the provided GUtil sub-namespace */
#define USING_NAMESPACE_GUTIL1( ns )            using namespace GUtil::ns
/** Use the GUtil namespace */
#define USING_NAMESPACE_GUTIL                   using namespace GUtil


#else

#define NAMESPACE_GUTIL2(a, b)
#define NAMESPACE_GUTIL1(a)
#define NAMESPACE_GUTIL

#define END_NAMESPACE_GUTIL2
#define END_NAMESPACE_GUTIL1
#define END_NAMESPACE_GUTIL

#define USING_NAMESPACE_GUTIL2(n1, n2)
#define USING_NAMESPACE_GUTIL1(n1)
#define USING_NAMESPACE_GUTIL

#endif // NO_GUTIL_NAMESPACES


/** Makes the copy constructor and assignment operators private.

    If a class takes a lot of work to copy, or for whatever other reason
    the designer of the class doesn't want you to use the copy constructor,
    this is a quick and easily readable way of declaring so.
*/
#define GUTIL_DISABLE_COPY(cls) \
    private: \
        cls(const cls &); \
        cls &operator =(const cls &);


/** Declares a class to be static, by making the assignment operator,
    default and copy constructors protected.

    If you have not defined any other constructors, then this essentially makes
    it impossible for anyone to instantiate the class.

    \note The defined constructors are protected, to allow static classes to derive
    from one another without compiler errors.

    \note Do not use this if the class is not actually static, because none of its
    members will be initialized in the constructor.
*/
#define GUTIL_STATIC_CLASS(cls) \
    protected: \
        cls(const cls &){} \
        cls &operator =(const cls &){return *this;} \
        cls(){}


/** A Macro that removes the warning from unused variables. */
#define GUTIL_UNUSED(x) (void)x;


#ifdef NULL
    #undef NULL
#endif

/** Defines the constant NULL as 0. */
#define NULL 0






#if defined(GUTIL_COM_EXPORTS)
    #define GUTIL_COM_DECLSPEC  __declspec(dllexport)
    #define GUTIL_COM_EXTERN    extern "C"

#elif defined(GUTIL_COM_IMPORTS)
    #define GUTIL_COM_DECLSPEC  __declspec(dllimport)
    #define GUTIL_COM_EXTERN    extern "C"

#else
    #define GUTIL_COM_DECLSPEC
    #define GUTIL_COM_EXTERN

#endif


#endif // GUTIL_MACROS_H
