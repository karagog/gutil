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

#ifndef GUTIL_EXTENDEDEXCEPTION_H
#define GUTIL_EXTENDEDEXCEPTION_H

/** \file
    This header file declares a template specialization for the Exception<bool>
    class, which adds many special features like being able to store strings
    and nested exceptions.
*/

#include "Core/exception.h"
#include "Core/DataObjects/vector.h"
#include "Core/DataObjects/map.h"
#include "Core/DataObjects/gstring.h"
GUTIL_BEGIN_NAMESPACE(Core);


/** Implements extended features for exception classes. */
class ExtendedException
{
public:

    ExtendedException();
    ExtendedException(const DataObjects::String &message,
                      const Exception<> *inner_exception = 0);

    ExtendedException(const ExtendedException &);
    ExtendedException &operator = (const ExtendedException &);
    virtual ~ExtendedException() throw();

    PROPERTY( Message, DataObjects::String );

    inline void SetData(const DataObjects::String &key, const DataObjects::String &value){
        _data[key] = value;
    }
    inline DataObjects::String GetData(const DataObjects::String &key) const{
        DataObjects::String ret;
        if(HasData(key)) ret = _data.At(key);
        return ret;
    }
    inline bool HasData(const DataObjects::String &key) const{ return _data.Contains(key); }

    /** Returns the map of key-value pairs. */
    const DataObjects::Map<DataObjects::String, DataObjects::String> &GetDataMap() const
    { return _data; }

    void SetInnerException(const Exception<> &ex);
    inline Exception<false> *GetInnerException() const{ return _inner_exception; }


private:

    DataObjects::Map<DataObjects::String, DataObjects::String> _data;
    Exception<> *_inner_exception;

};


#define STRINGIFY( something )  #something

/** An exception class that stores more data.

    This is a template specialization from the regular (non-extended) exception
    to an extended exception, which stores more complex data, including a message
    and a string-string map.
*/
#define EXCEPTION_DECLARE_EXTENDED( ex_name ) \
template<>class ex_name<true> : \
    public ex_name<false>, \
    public GUtil::Core::ExtendedException \
{ \
    public: \
\
        inline ex_name() {} \
        inline ex_name(const DataObjects::String &message) \
            :ExtendedException(message) {} \
        inline ex_name(const DataObjects::String &message, \
                         const char *file, \
                         int line, \
                         const Exception<> *inner_exception = 0) \
            :ex_name<false>(file, line), \
                ExtendedException(message, inner_exception) {} \
        inline ex_name(const Exception<> &ex, const ExtendedException &xex) \
            :ex_name<false>(ex), ExtendedException(xex) {} \
};

EXCEPTION_DECLARE_EXTENDED( Exception )
EXCEPTION_DECLARE_EXTENDED( NotImplementedException )
EXCEPTION_DECLARE_EXTENDED( BadAllocationException )
EXCEPTION_DECLARE_EXTENDED( ReadOnlyException )
EXCEPTION_DECLARE_EXTENDED( ArgumentException )
EXCEPTION_DECLARE_EXTENDED( DataTransportException )
EXCEPTION_DECLARE_EXTENDED( XmlException )
EXCEPTION_DECLARE_EXTENDED( EndOfFileException )
EXCEPTION_DECLARE_EXTENDED( LockException )
EXCEPTION_DECLARE_EXTENDED( NullReferenceException )
EXCEPTION_DECLARE_EXTENDED( IndexOutOfRangeException )
EXCEPTION_DECLARE_EXTENDED( ValidationException )
EXCEPTION_DECLARE_EXTENDED( InvalidCastException )
EXCEPTION_DECLARE_EXTENDED( NotFoundException )
EXCEPTION_DECLARE_EXTENDED( DivideByZeroException )
EXCEPTION_DECLARE_EXTENDED( UniqueKeyException )




/** This version lets you pass a message with the exception. */
#define THROW_NEW_GUTIL_EXCEPTION2( ex_type, message ) \
    throw ex_type<true>(message, __FILE__, __LINE__)

/** This version lets you pass an inner exception into it */
#define THROW_NEW_GUTIL_EXCEPTION3( ex_type, message, inner_except ) \
    throw ex_type<true>(message, __FILE__, __LINE__, inner_except)



GUTIL_END_NAMESPACE;

#endif // GUTIL_EXTENDEDEXCEPTION_H
