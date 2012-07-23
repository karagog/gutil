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

#ifndef GUTIL_EXTENDEDEXCEPTION_H
#define GUTIL_EXTENDEDEXCEPTION_H

/** \file
    This header file declares a template specialization for the Exception<bool>
    class, which adds many special features like being able to store strings
    and nested exceptions.
*/

#include "gutil_exception.h"
#include "gutil_vector.h"
#include "gutil_map.h"
#include "gutil_strings.h"

NAMESPACE_GUTIL


/** Implements extended features for exception classes. */
class ExtendedException
{
public:

    inline ExtendedException() :_inner_exception(0){}
    ExtendedException(const Exception<> &inner_exception);

    ExtendedException(const ExtendedException &);
    ExtendedException &operator = (const ExtendedException &);
    virtual ~ExtendedException();

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
    Exception<false> *_inner_exception;

};


#define STRINGIFY( something )  #something

/** An exception class that stores more data.

    This is a template specialization from the regular (non-extended) exception
    to an extended exception, which stores more complex data, including a message
    and a string-string map.
*/
#define GUTIL_EXCEPTION_DECLARE_EXTENDED( ex_name ) \
template<>class ex_name<true> : \
    public ex_name<false>, \
    public ExtendedException \
{ \
    public: \
\
        inline ex_name() :ex_name<false>(0, -1, "GUtil::" STRINGIFY(ex_name) "<true>"){} \
        inline ex_name(const char *message) :ex_name<false>(0, -1, "GUtil::" STRINGIFY(ex_name) "<true>", message){} \
        inline ex_name(const char *file, int line) \
            :ex_name<false>(file, line, "GUtil::" STRINGIFY(ex_name) "<true>") {} \
        inline ex_name(const char *file, \
                         int line, \
                         const char *message, \
                         const Exception<> &inner_exception) \
            :ex_name<false>(file, line, "GUtil::" STRINGIFY(ex_name) "<true>", message), \
                ExtendedException(inner_exception) {} \
}; \
extern template class ex_name<true>


GUTIL_EXCEPTION_DECLARE_EXTENDED( Exception );
GUTIL_EXCEPTION_DECLARE_EXTENDED( NotImplementedException );
GUTIL_EXCEPTION_DECLARE_EXTENDED( BadAllocationException );
GUTIL_EXCEPTION_DECLARE_EXTENDED( ReadOnlyException );
GUTIL_EXCEPTION_DECLARE_EXTENDED( ArgumentException );
GUTIL_EXCEPTION_DECLARE_EXTENDED( ConversionException );
GUTIL_EXCEPTION_DECLARE_EXTENDED( DataTransportException );
GUTIL_EXCEPTION_DECLARE_EXTENDED( XmlException );
GUTIL_EXCEPTION_DECLARE_EXTENDED( EndOfFileException );
GUTIL_EXCEPTION_DECLARE_EXTENDED( LockException );
GUTIL_EXCEPTION_DECLARE_EXTENDED( NullReferenceException );
GUTIL_EXCEPTION_DECLARE_EXTENDED( IndexOutOfRangeException );
GUTIL_EXCEPTION_DECLARE_EXTENDED( ValidationException );
GUTIL_EXCEPTION_DECLARE_EXTENDED( InvalidCastException );
GUTIL_EXCEPTION_DECLARE_EXTENDED( NotFoundException );
GUTIL_EXCEPTION_DECLARE_EXTENDED( DivideByZeroException );
GUTIL_EXCEPTION_DECLARE_EXTENDED( UniqueKeyException );
GUTIL_EXCEPTION_DECLARE_EXTENDED( BuildException );
GUTIL_EXCEPTION_DECLARE_EXTENDED( InvalidStateTransitionException );



/** This version lets you pass an inner exception into it */
#define THROW_NEW_GUTIL_EXCEPTION3( ex_type, message, inner_except ) \
    throw ex_type<true>(__FILE__, __LINE__, message, inner_except)



END_NAMESPACE_GUTIL

#endif // GUTIL_EXTENDEDEXCEPTION_H
