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

#ifndef GUTIL_EXCEPTION_H
#define GUTIL_EXCEPTION_H

#include "gutil_macros.h"
#include <exception>

#define STRINGIFY( str )  #str

/** The full form macro to declare an exception class, with all options.
 *  You can make an exception derived from a specified exception class and with custom members.
*/
#define GUTIL_EXCEPTION_DECLARE3(ex_name, ex_subclass_name, members) \
    template<bool extended = false>class ex_name : public ex_subclass_name<false> \
    { \
    public: \
        ex_name() \
            : ex_subclass_name<false>(0, -1) {} \
        ex_name(const char *message) \
            : ex_subclass_name<false>(0, -1, message) {} \
        ex_name(const char *file, int line, const char *message = 0) \
            : ex_subclass_name<false>(file, line, message) {} \
        ex_name(const ex_name<false> &ex) \
            : ex_subclass_name<false>(ex) {} \
        virtual ~ex_name() noexcept{} \
        virtual const char *what() const noexcept{ return "GUtil::" STRINGIFY(ex_name) "<false>"; } \
        virtual Exception<> *Clone() const noexcept{ return new ex_name<extended>(*this); } \
        members ; \
    }

/** You can use this to declare exceptions with public data members. */
#define GUTIL_EXCEPTION_DECLARE_WITH_MEMBERS( ex_name, members ) GUTIL_EXCEPTION_DECLARE3(ex_name, GUtil::Exception, members)


/** Use this to declare simple exception types with no extra public members. */
#define GUTIL_EXCEPTION_DECLARE( ex_name ) GUTIL_EXCEPTION_DECLARE_WITH_MEMBERS(ex_name, private:)


/** Use this to declare exceptions, which derive from other types of exceptions. */
#define GUTIL_EXCEPTION_DECLARE2( ex_name, ex_subclass_name ) GUTIL_EXCEPTION_DECLARE3(ex_name, ex_subclass_name, private:)


NAMESPACE_GUTIL;


/** We define a base class, so that the construction code is not reproduced
    for each exception declaration.  That would be useless code bloat, so in the Exception
    class we all the constructors and they call the base constructors which
    are defined in the lib.

    It derives from the standard exception, so all exceptions can be handled by catching the
    standard exception.
*/
class BaseException : public std::exception
{
    char *m_file;
    int m_line;
    char *m_message;

    /** We allow this function to manipulate the file and line info for this class. */
    friend void __setExceptionFileAndLineInfo(BaseException &, const char *, int);

public:

    /** Use this constructor to inject more information in your exception. */
    BaseException(const char *message = 0, const char *file = 0, int line = -1) noexcept;
    BaseException(const BaseException &o) noexcept;
    BaseException &operator = (const BaseException &o) noexcept;
    virtual ~BaseException() noexcept;

    /** You can pass the preprocessor macro __FILE__ into the constructor and it
        will be stored here.
        \note You should check that it's non-zero before using.
    */
    const char *File() const;

    /** You can pass the preprocessor macro __LINE__ into the constructor and it
        will be stored here.
        \note Will be -1 if not specified
    */
    int Line() const;

    /** You can include a null-terminated message with the exception. */
    const char *Message() const;

};


/** The base class for all GUtil exceptions.

    Every other type of exception is derived from this.

    The default exception is non-extended, meaning it only contains the most
    basic information, like the file and line numbers, as well as a string identifier.
    You can use the extended version to include more complex data in your exceptions.
*/
template<bool extended = false> class Exception;

template<>
class Exception<false> : public BaseException
{
public:

    /** Constructs an exception.  Normally you don't construct one directly, you use
        THROW_NEW_GUTIL_EXCEPTION for convenience.

        \param file The file name for which the exception is thrown.  This MUST be a literal string.
                        You are supposed to use the __FILE__ preprocessor macro.
        \param line The line on which the exception was thrown.  You are supposed to use __LINE__
        \param message A message for the exception.  Unlike the other string parameters, this one
                        does not need to be a literal string, as it is deep-copied into the exception.
    */
    Exception(const char *file, int line, const char *message = 0);
    Exception(){}

    virtual const char *what() const noexcept;
    virtual Exception<> *Clone() const noexcept;
    virtual ~Exception() noexcept;

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

/** Means that an operation failed due to authentication problems. */
GUTIL_EXCEPTION_DECLARE( AuthenticationException );

/** Means that an operation was cancelled. */
GUTIL_EXCEPTION_DECLARE( CancelledOperationException );

/** Means that an operation failed due to a bad build.  This is a compile-time bug,
    and probably means the developer used the wrong preprocessor definitions or something.
*/
GUTIL_EXCEPTION_DECLARE( BuildException );

/** Means that someone tried an invalid state transition. */
GUTIL_EXCEPTION_DECLARE( InvalidStateTransitionException );



/** This friend of the exception class is used to override the file and line
 *  info for an exception after instantiation.  You should use the
*/
void __setExceptionFileAndLineInfo(BaseException &, const char *file, int line);

/** Use this convenient macro to insert the file/line data in the exception */
#define THROW_GUTIL_EXCEPTION( except ) \
    GUtil::__setExceptionFileAndLineInfo(except, __FILE__, __LINE__); \
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
    throw ex_type<false>(__FILE__, __LINE__, msg)


/** You can use this to instantiate a new exception with the correct line and file information,
 *  thus allowing you to modify it before throwing it.
*/
#define INSTANTIATE_GUTIL_EXCEPTION(ex_type, var_name) \
    ex_type<false> var_name(__FILE_, __LINE__)

/** You can use this to instantiate a new exception with a message and the correct line and file information,
 *  thus allowing you to modify it before throwing it.
*/
#define INSTANTIATE_GUTIL_EXCEPTION2(ex_type, var_name, msg) \
    ex_type<false> var_name(__FILE_, __LINE__, msg)


END_NAMESPACE_GUTIL;

#endif // GEXEPTION_H
