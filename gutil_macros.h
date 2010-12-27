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

#ifndef GUTIL_MACROS_H
#define GUTIL_MACROS_H

// Here are some useful macros for you to use:


// Use this to succinctly declare property accessors
//   It declares a Get and Set method for the provided variable type and name,
//   and also a unique private variable to hold the value
#define PROPERTY( name, type ) \
private: \
    type _p_##name; \
public: \
    inline type Get##name() const{ return _p_##name; } \
    inline void Set##name(const type &value){ _p_##name = value; } \
    enum{}

#define PROPERTY_POINTER( name, ptr_type ) \
private: \
    ptr_type *_p_##name; \
public: \
    inline ptr_type *Get##name() const{ return _p_##name; } \
    inline void Set##name(ptr_type *value){ _p_##name = value; } \
    enum{}


#define PROTECTED_PROPERTY( name, type ) \
private: \
    type _p_##name; \
protected: \
    inline type Get##name() const{ return _p_##name; } \
    inline void Set##name(const type &value){ _p_##name = value; } \
    enum{}


// Only declares a Getter for the hidden property
#define READONLY_PROPERTY( name, type ) \
private: \
    type _p_##name; \
public: \
    inline type Get##name() const{ return _p_##name; } \
    enum{}


#define PROTECTED_READONLY_PROPERTY( name, type ) \
private: \
    type _p_##name; \
protected: \
    inline type Get##name() const{ return _p_##name; } \
    enum{}


// Macros to begin and end namespaces
        // IMPORTANT!  QMake does not evaluate preprocessor definitions
        //  when creating meta-object files, so you cannot use these
        //  to put any object derived from QObject in a namespace

// Also note that you must terminate these with a semi-colon
#define GUTIL_BEGIN_NAMESPACE( n ) \
        namespace GUtil \
        { \
          namespace n \
          {

#define GUTIL_BEGIN_CORE_NAMESPACE( n ) \
        namespace GUtil \
        { \
          namespace Core \
          { \
            namespace n \
            {

#define GUTIL_END_NAMESPACE }}
#define GUTIL_END_CORE_NAMESPACE }}}

#define BEGIN_NAMESPACE( ns ) namespace ns{
#define END_NAMESPACE }


#define GUTIL_USING_NAMESPACE( name ) using namespace GUtil::name
#define GUTIL_USING_CORE_NAMESPACE( name ) using namespace GUtil::Core::name

#endif // GUTIL_MACROS_H
