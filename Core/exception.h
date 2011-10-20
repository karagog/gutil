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

namespace GUtil{ namespace Core{

template<bool extended = false>class Exception{};

/** The base class for all of my exceptions.

    Every other type of exception is derived from this.

    The default exception is non-extended, meaning it only contains the most
    basic information, like the file and line numbers, as well as a string identifier.
    You can use the extended version to include more complex data in your exceptions.
*/
template<>class Exception<false>
{
public:

    inline Exception()
        :Name("GUtil::Core::Exception"), File(0), Line(-1){}

    /** Use this constructor to inject more information in your exception. */
    inline Exception(const char *name)
        :Name(name), File(0), Line(-1){}

    /** Use this constructor to inject more information in your exception. */
    inline Exception(const char *name, const char *file, int line)
        :Name(name), File(file), Line(line){}

    /** The destructor is virtual, so it will have RTTI (Run Time Type Info) on it.
        This will allow you to dynamic_cast a reference to an Exception as a different
        type of exception at runtime.
    */
    virtual ~Exception() throw(){}

    /** The name of the exception, injected by the constructor. */
    const char *Name;

    /** You can pass the preprocessor macro __FILE__ into the constructor and it
        will be stored here.
    */
    const char *File;

    /** You can pass the preprocessor macro __LINE__ into the constructor and it
        will be stored here.
    */
    int Line;

};



#define STRINGIFY( something )  #something

/** Use this to declare any new exceptions */
#define EXCEPTION_DECLARE( ex_name ) \
template<bool extended = false>class ex_name##Exception{}; \
template<>class ex_name##Exception<false>: \
    public GUtil::Core::Exception<false> \
{ \
public: \
    ex_name##Exception() \
        :Exception(STRINGIFY(ex_name)){} \
    ex_name##Exception(const char *file, int line) \
        :Exception(STRINGIFY(ex_name), file, line){} \
};


// Here are the other types of exceptions (all derived from Exception)
EXCEPTION_DECLARE( NotImplemented )
EXCEPTION_DECLARE( BadAllocation )
EXCEPTION_DECLARE( ReadOnly )
EXCEPTION_DECLARE( Argument )
EXCEPTION_DECLARE( DataTransport )
EXCEPTION_DECLARE( Xml )
EXCEPTION_DECLARE( EndOfFile )
EXCEPTION_DECLARE( Lock )
EXCEPTION_DECLARE( NullReference )
EXCEPTION_DECLARE( IndexOutOfRange )
EXCEPTION_DECLARE( Validation )
EXCEPTION_DECLARE( InvalidCast )
EXCEPTION_DECLARE( NotFound )
EXCEPTION_DECLARE( DivideByZero )
EXCEPTION_DECLARE( UniqueKey )



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


}}  // GUtil::Core

#endif // GEXEPTION_H
