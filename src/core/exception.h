/*Copyright 2010-2014 George Karagoulis

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

#include <gutil/iclonable.h>
#include <exception>
#include <string>
#include <unordered_map>
#include <memory>
#include <initializer_list>

#define STRINGIFY( str )  #str

/** The full form macro to declare an exception class, with all options.
 *  You can make an exception derived from a specified exception class and with custom members.
*/
#define GUTIL_EXCEPTION_DECLARE_BASE3(ex_name, ex_subclass_name, members) \
    template<bool extended = false>class ex_name : public ex_subclass_name { \
    public: \
        inline ex_name(const char *message = "") \
            : ex_subclass_name(message) {} \
        inline ex_name(const ex_name<false> &ex) \
            : ex_subclass_name(ex) {} \
        virtual ~ex_name() noexcept{} \
        virtual const char *what() const noexcept{ return "GUtil::" STRINGIFY(ex_name) "<>"; } \
        virtual IClonable *Clone() const noexcept{ return new ex_name<false>(*this); } \
        members ; \
    }
    
/** Use this to declare extended exception types, which hold more complex data.

    This is a template specialization from the regular (non-extended) exception
    to an extended exception, which stores more complex data, including a message
    and a string-string map.

    \note You must have already declared the non-extended version of the exception
*/
#define GUTIL_EXCEPTION_DECLARE_EXTENDED( ex_name ) \
template<>class ex_name<true> : public ex_name<false>, public GUtil::ExtendedException { \
        public: \
            ex_name() {} \
            ex_name(const char *message) :ex_name<false>(message) {} \
            ex_name(const char *message, std::initializer_list<std::pair<const std::string, std::string>> il) \
                :ex_name<false>(message), ExtendedException(il) {} \
            ex_name(std::initializer_list<std::pair<const std::string, std::string>> il) \
                :ExtendedException(il) {} \
            ex_name(const char *message, const GUtil::Exception<> &inner_exception, \
                    std::initializer_list<std::pair<const std::string, std::string>> il) \
                :ex_name<false>(message), ExtendedException(il, inner_exception) {} \
            ex_name(const char *message, const GUtil::Exception<> &inner_exception) \
                :ex_name<false>(message), ExtendedException(inner_exception) {} \
            virtual ~ex_name() noexcept {} \
            virtual const char *what() const noexcept{ return "GUtil::" STRINGIFY(ex_name) "<true>"; } \
            virtual Exception<> *Clone() const noexcept{ return new ex_name<true>(*this); } \
    }

    
/** Use this to declare only the base type of exception, derived from the given subclass. */
#define GUTIL_EXCEPTION_DECLARE_BASE2( ex_name, ex_subclass_name ) GUTIL_EXCEPTION_DECLARE_BASE3(ex_name, ex_subclass_name, private:)

/** Use this to declare only the base type of exception. */
#define GUTIL_EXCEPTION_DECLARE_BASE( ex_name ) GUTIL_EXCEPTION_DECLARE_BASE2(ex_name, GUtil::Exception<false>)

/** You can use this to declare exceptions (base and extended) which derive from the given base class and 
    have the given public data members.
*/
#define GUTIL_EXCEPTION_DECLARE3( ex_name, ex_subclass_name, members ) \
    GUTIL_EXCEPTION_DECLARE_BASE3(ex_name, ex_subclass_name, members); \
    GUTIL_EXCEPTION_DECLARE_EXTENDED(ex_name)
    
/** You can use this to declare exceptions (base and extended) which derive from the given base class and 
    have the given public data members.
*/
#define GUTIL_EXCEPTION_DECLARE2( ex_name, ex_subclass_name ) \
    GUTIL_EXCEPTION_DECLARE_BASE2(ex_name, ex_subclass_name); \
    GUTIL_EXCEPTION_DECLARE_EXTENDED(ex_name)

/** Use this to declare both the base and extended types of exceptions. */
#define GUTIL_EXCEPTION_DECLARE( ex_name ) \
    GUTIL_EXCEPTION_DECLARE_BASE(ex_name); \
    GUTIL_EXCEPTION_DECLARE_EXTENDED(ex_name)


NAMESPACE_GUTIL;


/** We define a base class, so that the construction code is not reproduced
    for each exception declaration.  That would be useless code bloat, so in the Exception
    class we all the constructors and they call the base constructors which
    are defined in the lib.

    It derives from the standard exception, so all exceptions can be handled by catching the
    standard exception.
*/
class BaseException : public std::exception, public IClonable {
    const std::string m_message;
public:
    /** Returns the exception message (may be empty). */
    inline const std::string &Message() const{ return m_message; }
    virtual ~BaseException() noexcept {}
protected:
    /** Use this constructor to inject a message in your exception. */
    BaseException(const char *message) noexcept :m_message(message) {}
};


/** The base class for all GUtil exceptions.

    Every other type of exception is derived from this.

    The default exception is non-extended, meaning it only contains the most
    basic information, like the file and line numbers, as well as a string identifier.
    You can use the extended version to include more complex data in your exceptions.
*/
GUTIL_EXCEPTION_DECLARE_BASE2(Exception, BaseException);


/** Implements extended features for exception classes. */
class ExtendedException {
    std::unique_ptr<Exception<>> m_innerException;
public:
    /** Directly access the exception data map. */
    std::unordered_map<std::string, std::string> Data;

    /** Access the exception's inner exception (may be null). */
    inline Exception<> *GetInnerException() const{ return m_innerException.get(); }
    virtual ~ExtendedException() {}
protected:
    ExtendedException() {}
    ExtendedException(std::initializer_list<std::pair<const std::string, std::string>> il)
        :Data(il) {}
    ExtendedException(std::initializer_list<std::pair<const std::string, std::string>> il, const Exception<> &inner_exception)
        :m_innerException((Exception<>*)inner_exception.Clone()), Data(il) {}
    ExtendedException(const Exception<> &inner_exception)
        :m_innerException((Exception<>*)inner_exception.Clone()) {}
    ExtendedException(const ExtendedException &o)
        :m_innerException(o.m_innerException ? (Exception<>*)o.m_innerException->Clone() : NULL),
          Data(o.Data) {}
    ExtendedException &operator = (const ExtendedException &o){
        if(o.m_innerException) m_innerException.reset((Exception<> *)o.GetInnerException()->Clone());
        else m_innerException.reset(NULL);
        Data = o.Data;
        return *this;
    }
};

/** The extended GUtil exception. */
GUTIL_EXCEPTION_DECLARE_EXTENDED(Exception);


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


END_NAMESPACE_GUTIL;

#endif // GUTIL_EXCEPTION_H
