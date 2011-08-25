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

#ifndef GUTIL_MACROS_H
#define GUTIL_MACROS_H

/** \file

    Here are some useful macros for you to use.

    They are for creating property accessors, using GUtil namespaces and
    whatever else might be useful from a macro standpoint.
*/



/** Use this to succinctly declare property accessors.

   It declares a Get and Set method for an object of the provided variable type and name,
   and also a private variable to hold the value.
*/
#define PROPERTY( name, type ) \
private: \
    type _p_##name; \
public: \
    inline const type &Get##name() const{ return _p_##name; } \
    inline void Set##name(const type &value){ _p_##name = value; }


/** Use this to succinctly declare property accessors to a pointer.

    It declares a Get and Set method for a pointer to an object of
    the provided variable type and name, and also a private variable
    to hold the value.
*/
#define PROPERTY_POINTER( name, ptr_type ) \
private: \
    ptr_type *_p_##name; \
public: \
    inline ptr_type *Get##name() const{ return _p_##name; } \
    inline void Set##name(ptr_type *value){ _p_##name = value; }


/** Use this to succinctly declare protected property accessors.

   It declares a Get and Set method for an object of the provided variable type and name,
   and also a private variable to hold the value.
*/
#define PROTECTED_PROPERTY( name, type ) \
private: \
    type _p_##name; \
protected: \
    inline const type &Get##name() const{ return _p_##name; } \
    inline void Set##name(const type &value){ _p_##name = value; }


/** Use this to succinctly declare a readonly property accessor.

   It declares a Get method for an object of the provided variable type and name,
   but does not give a Set method with which to change the value.
*/
#define READONLY_PROPERTY( name, type ) \
private: \
    type _p_##name; \
public: \
    inline const type &Get##name() const{ return _p_##name; }


/** Use this to succinctly declare a readonly property accessor to a pointer.

    It declares a Get method for a pointer to an object of
    the provided variable type and name, and also a private variable
    to hold the value.
*/
#define READONLY_PROPERTY_POINTER( name, type ) \
private: \
    type *_p_##name; \
public: \
    inline type *Get##name() const{ return _p_##name; }


/** Use this to succinctly declare a readonly protected property accessor.

   It declares a Get method for an object of the provided variable type and name,
   and also a private variable to hold the value.
*/
#define PROTECTED_READONLY_PROPERTY( name, type ) \
private: \
    type _p_##name; \
protected: \
    inline const type &Get##name() const{ return _p_##name; }




/** A macro to begin a GUtil namespace.

    IMPORTANT!  QMake does not evaluate preprocessor definitions
    when creating meta-object files, so you cannot use these
    to put any object derived from QObject in a namespace.
*/
#define GUTIL_BEGIN_NAMESPACE( n ) \
        namespace GUtil \
        { \
          namespace n \
          {


/** A macro to begin a GUtil::Core namespace. */
#define GUTIL_BEGIN_CORE_NAMESPACE( n ) \
        namespace GUtil \
        { \
          namespace Core \
          { \
            namespace n \
            {


/** Ends a GUtil namespace. */
#define GUTIL_END_NAMESPACE }}
/** Ends a GUtil::Core namespace. */
#define GUTIL_END_CORE_NAMESPACE }}}

/** Begins a generic namespace declaration.

    This could be useful if you wanted to be able to switch on/off the use of namespaces
*/
#define BEGIN_NAMESPACE( ns ) namespace ns{
/** Ends a generic namespace declaration. */
#define END_NAMESPACE }


/** Use the provided GUtil namespace */
#define GUTIL_USING_NAMESPACE( name ) using namespace GUtil::name
/** Use the provided GUtil::Core namespace */
#define GUTIL_USING_CORE_NAMESPACE( name ) using namespace GUtil::Core::name




// These macros only relevant for windows COM functionality
#ifdef __WIN32__

    /** Preps a function to be exported in a COM interface
            (no member functions; it is strictly C style).

        It is declared extern "C" to avoid name-mangling of the function name, so you
        can call functions directly by their name.  You can even call the native functions in managed (C#) code.
    */
    #ifdef GUTIL_COM_EXPORTS
        #define GUTIL_COM_EXPORT extern "C" __declspec(dllexport)
    #else
        #define GUTIL_COM_EXPORT
    #endif

    /** Declares a function as a dll export (name mangling still applies). */
    #ifdef GUTIL_DLL_EXPORTS
        #define GUTIL_DLL_EXPORT __declspec(dllexport)
    #else
        #define GUTIL_DLL_EXPORT
    #endif
#else
    #define GUTIL_DLL_EXPORT
    #define GUTIL_COM_EXPORT
#endif



#endif // GUTIL_MACROS_H
