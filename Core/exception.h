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

#ifndef GUTIL_EXCEPTION_H
#define GUTIL_EXCEPTION_H

#include "gutil_macros.h"

namespace GUtil{ namespace Core{


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
    virtual ~BaseException() throw();

};


/** The base class for all of my exceptions.

    Every other type of exception is derived from this.

    The default exception is non-extended, meaning it only contains the most
    basic information, like the file and line numbers, as well as a string identifier.
    You can use the extended version to include more complex data in your exceptions.
*/
template<bool extended = false>
class Exception :
        public BaseException
{
public:
    inline Exception() :BaseException("GUtil::Core::Exception<false>"){}
    inline Exception(const char *message, const char *name = 0) :BaseException(name == 0 ? "GUtil::Core::Exception<false>" : name, message) {} \
    inline Exception(const char *file,
                     int line,
                     const char *name = 0,
                     const char *message = 0)
        :BaseException(name == 0 ? "GUtil::Core::Exception<false>" : name, message, file, line) {}
};



#define STRINGIFY( something )  #something

/** Use this to declare any new exceptions */
#define EXCEPTION_DECLARE( ex_name ) \
template<bool extended = false>class ex_name : public Exception<false> \
{ \
public: \
    inline ex_name() \
        :Exception<false>(0, -1, "GUtil::Core::" STRINGIFY(ex_name) "<false>") {} \
    inline ex_name(const char *message, const char *name) \
        :Exception<false>(0, -1, name == 0 ? "GUtil::Core::" STRINGIFY(ex_name) "<false>" : name, message) {} \
    inline ex_name(const char *file, int line, const char *name = 0, const char *message = 0) \
        :Exception<false>(file, line, name == 0 ? "GUtil::Core::" STRINGIFY(ex_name) "<false>" : name, message) {} \
    inline ex_name(const Exception<false> &ex) \
        :Exception<false>(ex) {} \
};


// Here are the other types of exceptions (all derived from Exception)
EXCEPTION_DECLARE( NotImplementedException )
EXCEPTION_DECLARE( BadAllocationException )
EXCEPTION_DECLARE( ReadOnlyException )
EXCEPTION_DECLARE( ArgumentException )
EXCEPTION_DECLARE( DataTransportException )
EXCEPTION_DECLARE( XmlException )
EXCEPTION_DECLARE( EndOfFileException )
EXCEPTION_DECLARE( LockException )
EXCEPTION_DECLARE( NullReferenceException )
EXCEPTION_DECLARE( IndexOutOfRangeException )
EXCEPTION_DECLARE( ValidationException )
EXCEPTION_DECLARE( InvalidCastException )
EXCEPTION_DECLARE( NotFoundException )
EXCEPTION_DECLARE( DivideByZeroException )
EXCEPTION_DECLARE( UniqueKeyException )



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
    throw ex_type<false>(__FILE__, __LINE__, msg)


}}  // GUtil::Core

#endif // GEXEPTION_H
