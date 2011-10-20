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

#include "Core/exception.h"
#include "Core/DataObjects/vector.h"
#include "Core/DataObjects/map.h"
#include "Core/DataObjects/gstring.h"
GUTIL_BEGIN_NAMESPACE(Core);


/** An exception class that stores more data.

    This is a template specialization from the regular (non-extended) exception.
*/
template<>class Exception<true> : public Exception<false>
{
public:

    inline Exception()
        :Exception<false>("GUtil::Core::ExtendedException"), _inner_exception(0){}
    inline Exception(const DataObjects::String &message)
        :Exception<false>("GUtil::Core::ExtendedException"),
          _p_Message(message),
          _inner_exception(0){}
    inline Exception(const DataObjects::String &message,
                     const char *file,
                     int line,
                     Exception *inner_exception = 0)
        :Exception<false>("GUtil::Core::ExtendedException", file, line),
          _p_Message(message),
          _inner_exception(_inner_exception ? new Exception(*inner_exception) : 0){}
    inline Exception(const Exception<true> &o)
        :Exception<false>(o),
          _p_Message(o._p_Message),
          _data(o._data),
          _inner_exception(o._inner_exception ? new Exception(*o._inner_exception) : 0){}
    virtual ~Exception() throw(){
        if(_inner_exception)
            delete _inner_exception;
    }

    PROPERTY( Message, DataObjects::String );

    inline void SetData(const DataObjects::String &key, const DataObjects::String &value){
        _data.At(key) = value;
    }

    inline DataObjects::String GetData(const DataObjects::String &key) const{
        return _data.At(key);
    }

    // Get a list of the keys you've put in the data collection
    DataObjects::Vector<DataObjects::String> GetDataKeys(bool include_blanks = false) const{
        DataObjects::Vector<DataObjects::String> ret;

        for(DataObjects::Map<DataObjects::String, DataObjects::String>::const_iterator
                it = _data.begin(); it; ++it)
        {
            DataObjects::String first = it->Key();
            DataObjects::String second = it->Value();

            if(include_blanks || second.Length() > 0)
                ret.PushBack(first);
        }

        return ret;
    }

    DataObjects::String ToString() const;

    inline void SetInnerException(const Exception &ex){
        if(_inner_exception)
            delete _inner_exception;
        _inner_exception = new Exception(ex);
    }

    inline Exception *GetInnerException() const{ return _inner_exception; }


protected:

    inline Exception(const DataObjects::String &identifier,
                     const DataObjects::String &message,
                     const char *file = 0,
                     int line = -1,
                     Exception *inner_exception = 0)
        :Exception<false>((_identifier = "GUtil::Core::Extended").Append(identifier).Append("Exception").ConstData(),
                          file, line),
          _p_Message(message),
          _inner_exception(_inner_exception ? new Exception(*inner_exception) : 0){}


private:

    DataObjects::String _identifier;
    DataObjects::Map<DataObjects::String, DataObjects::String> _data;

    Exception *_inner_exception;

};



#define STRINGIFY( something )  #something

/** Use this to declare any new exceptions */
#define EXCEPTION_DECLARE_EXTENDED( ex_name ) \
template<>class ex_name##Exception<true> : public GUtil::Core::Exception<true> \
{ \
public: \
    ex_name##Exception() \
        :Exception<true>(STRINGIFY(ex_name)){} \
    ex_name##Exception(const char *msg) \
        :Exception<true>(STRINGIFY(ex_name), msg){} \
    ex_name##Exception(const char *msg, const char *file, int line, Exception *ie = 0) \
        :Exception<true>(STRINGIFY(ex_name), msg, file, line, ie){} \
};

EXCEPTION_DECLARE_EXTENDED( NotImplemented )
EXCEPTION_DECLARE_EXTENDED( BadAllocation )
EXCEPTION_DECLARE_EXTENDED( ReadOnly )
EXCEPTION_DECLARE_EXTENDED( Argument )
EXCEPTION_DECLARE_EXTENDED( DataTransport )
EXCEPTION_DECLARE_EXTENDED( Xml )
EXCEPTION_DECLARE_EXTENDED( EndOfFile )
EXCEPTION_DECLARE_EXTENDED( Lock )
EXCEPTION_DECLARE_EXTENDED( NullReference )
EXCEPTION_DECLARE_EXTENDED( IndexOutOfRange )
EXCEPTION_DECLARE_EXTENDED( Validation )
EXCEPTION_DECLARE_EXTENDED( InvalidCast )
EXCEPTION_DECLARE_EXTENDED( NotFound )
EXCEPTION_DECLARE_EXTENDED( DivideByZero )
EXCEPTION_DECLARE_EXTENDED( UniqueKey )




/** This version lets you pass a message with the exception. */
#define THROW_NEW_GUTIL_EXCEPTION2( ex_type, message ) \
    throw ex_type<true>(message, __FILE__, __LINE__)

/** This version lets you pass an inner exception into it */
#define THROW_NEW_GUTIL_EXCEPTION3( ex_type, message, inner_except ) \
    throw ex_type<true>(message, __FILE__, __LINE__, inner_except)



GUTIL_END_NAMESPACE;

#endif // GUTIL_EXTENDEDEXCEPTION_H
