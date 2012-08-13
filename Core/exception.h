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

#ifndef GUTIL_EXCEPTION_H
#define GUTIL_EXCEPTION_H

#include "gutil_macros.h"


#define STRINGIFY( something )  #something

/** Use this to declare any new exceptions */
#define GUTIL_EXCEPTION_DECLARE( ex_name ) \
template<bool extended = false>class ex_name : public ::GUtil::Exception<false> \
{ \
public: \
    inline ex_name() \
        : ::GUtil::Exception<false>(0, -1, "GUtil::" STRINGIFY(ex_name) "<false>") {} \
    inline ex_name(const char *message, const char *name = 0) \
        : ::GUtil::Exception<false>(0, -1, name == 0 ? "GUtil::" STRINGIFY(ex_name) "<false>" : name, message) {} \
    inline ex_name(const char *file, int line, const char *name = 0, const char *message = 0) \
        : ::GUtil::Exception<false>(file, line, name == 0 ? "GUtil::" STRINGIFY(ex_name) "<false>" : name, message) {} \
    inline ex_name(const ex_name<false> &ex) \
        : ::GUtil::Exception<false>(ex) {} \
    virtual ~ex_name(){} \
}; \
extern template class ex_name<false>


/** Use this to declare any new exceptions, which derive from other types of exceptions */
#define GUTIL_EXCEPTION_DECLARE2( ex_name, ex_subclass_name ) \
template<bool extended = false>class ex_name : public ex_subclass_name<false> \
{ \
public: \
    inline ex_name() \
        :ex_subclass_name<false>(0, -1, "GUtil::" STRINGIFY(ex_name) "<false>") {} \
    inline ex_name(const char *message, const char *name = 0) \
        :ex_subclass_name<false>(0, -1, name == 0 ? "GUtil::" STRINGIFY(ex_name) "<false>" : name, message) {} \
    inline ex_name(const char *file, int line, const char *name = 0, const char *message = 0) \
        :ex_subclass_name<false>(file, line, name == 0 ? "GUtil::" STRINGIFY(ex_name) "<false>" : name, message) {} \
    inline ex_name(const ex_name<false> &ex) \
        :ex_subclass_name<false>(ex) {} \
    virtual ~ex_name(){} \
}; \
extern template class ex_name<false>


/** For instantiating exceptions for export.  You put this in the C file to
 *  instantiate the code there.  You must also declare GUTIL_EXCEPTION_DECLARE
 *  in the header for this to be useful

    This decreases the size of all dependent libraries/executables, because
    the exception code doesn't have to be repeated
*/
#define GUTIL_EXCEPTION_INSTANTIATE(EX_TYPE)  template class EX_TYPE<false>



NAMESPACE_GUTIL


/** We define a base class, so that the construction code is not reproduced
    for each exception declaration.  That would be useless code bloat, so in the Exception
    class we inline all the constructors and they call the base constructors which
    are defined in the lib.

    \note If you catch this type of exception, you will effectively catch any kind of GUtil exception.
*/
class BaseException
{
    char *m_message;
public:

    /** Use this constructor to inject more information in your exception. */
    BaseException(const char *name, const char *message = 0, const char *file = 0, int line = -1);

    BaseException(const BaseException &o);
    BaseException &operator = (const BaseException &o);

    /** The name of the exception, injected by the constructor.
        \note You should check that it's non-zero before using.
    */
    const char *What;

    /** You can pass the preprocessor macro __FILE__ into the constructor and it
        will be stored here.
        \note You should check that it's non-zero before using.
    */
    const char *File;

    /** You can pass the preprocessor macro __LINE__ into the constructor and it
        will be stored here.
        \note Will be -1 if not specified
    */
    int Line;

    /** You can include a null-terminated message with the exception. */
    inline const char *GetMessage() const{ return m_message; }

    /** The destructor is virtual, so it will have RTTI (Run Time Type Info) on it.
        This will allow you to dynamic_cast a reference to an Exception as a different
        type of exception at runtime.
    */
    virtual ~BaseException();

};


/** The base class for all of my exceptions.

    Every other type of exception is derived from this.

    The default exception is non-extended, meaning it only contains the most
    basic information, like the file and line numbers, as well as a string identifier.
    You can use the extended version to include more complex data in your exceptions.
*/
template<bool extended = false> class Exception;

template<>
class Exception<false> :
        public BaseException
{
public:
    Exception();
    virtual ~Exception(){}
    explicit Exception(const char *message, const char *name = 0);
    Exception(const char *file, int line, const char *name = 0, const char *message = 0);
};



// Here are the other types of exceptions (all derived from Exception)

/** Means the given code path is not implemented */
GUTIL_EXCEPTION_DECLARE( NotImplementedException );

/** Means that there was an error allocating memory */
GUTIL_EXCEPTION_DECLARE( BadAllocationException );

/** Means that an operation failed because an object was in a readonly state */
GUTIL_EXCEPTION_DECLARE( ReadOnlyException );

/** Means that an operation failed due to bad arguments */
GUTIL_EXCEPTION_DECLARE( ArgumentException );

/** Means that an operation failed due to a failed conversion (string->int, for example) */
GUTIL_EXCEPTION_DECLARE( ConversionException );

/** Means that an operation failed due to failed data transportation */
GUTIL_EXCEPTION_DECLARE( DataTransportException );

/** Means that an operation failed due to an XML error (bad/unexpected format) */
GUTIL_EXCEPTION_DECLARE( XmlException );

/** Means that an operation failed because the end of a file was encountered */
GUTIL_EXCEPTION_DECLARE( EndOfFileException );

/** Means that a lock operation failed */
GUTIL_EXCEPTION_DECLARE( LockException );

/** Means that an operation failed because a null pointer/reference was used */
GUTIL_EXCEPTION_DECLARE( NullReferenceException );

/** Means that an operation failed because an index was out of range */
GUTIL_EXCEPTION_DECLARE( IndexOutOfRangeException );

/** Means that a validation operation failed */
GUTIL_EXCEPTION_DECLARE( ValidationException );

/** Means that a cast operation failed */
GUTIL_EXCEPTION_DECLARE( InvalidCastException );

/** Means that an operation failed because an object was not found */
GUTIL_EXCEPTION_DECLARE( NotFoundException );

/** Means that an operation failed because it would have to divide by 0 */
GUTIL_EXCEPTION_DECLARE( DivideByZeroException );

/** Means that an operation failed due to a unique key violation */
GUTIL_EXCEPTION_DECLARE( UniqueKeyException );

/** Means that an operation failed due to a bad build.  This is a compile-time bug,
    and probably means the developer used the wrong preprocessor definitions or something.
*/
GUTIL_EXCEPTION_DECLARE( BuildException );

/** Means that someone tried an invalid state transition. */
GUTIL_EXCEPTION_DECLARE( InvalidStateTransitionException );



/** Use this convenient macro to insert the file/line data in the exception */
#define THROW_GUTIL_EXCEPTION( except ) \
    except.File = __FILE__; \
    except.Line = __LINE__; \
    throw except

/** Use this convenient macro to instantiate an exception of the specified type,
    and pass the file/line data with it.
*/
#define THROW_NEW_GUTIL_EXCEPTION( ex_type ) \
    throw ex_type<false>(__FILE__, __LINE__)

/** Use this convenient macro to instantiate an exception of the specified type,
    and pass the file/line data with it, along with a custom message.
*/
#define THROW_NEW_GUTIL_EXCEPTION2( ex_type, msg ) \
    throw ex_type<false>(__FILE__, __LINE__, 0, msg)


END_NAMESPACE_GUTIL

#endif // GEXEPTION_H
