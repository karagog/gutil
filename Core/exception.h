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

#ifndef GEXCEPTION_H
#define GEXCEPTION_H

#include "gutil.h"
#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

// Use these convenient macros to insert the file/line data in the exception
#define THROW_GUTIL_EXCEPTION( except ) \
do{ \
    char tmp_except_line[15];  \
    sprintf(tmp_except_line, "%d", __LINE__); \
    except.SetData("line", tmp_except_line); \
    except.SetData("file", __FILE__); \
    throw except; \
}while(0)


#define THROW_NEW_GUTIL_EXCEPTION( ex_type ) \
do{ \
    throw ex_type(""); \
}while(0)

// pass an exception type and the message you want it to have
#define THROW_NEW_GUTIL_EXCEPTION2( ex_type, message ) \
do{ \
    ex_type except(message); \
\
    char tmp_except_line[15];  \
    sprintf(tmp_except_line, "%d", __LINE__); \
    except.SetData("line", tmp_except_line); \
    except.SetData("file", __FILE__); \
    throw except; \
}while(0)

// This version lets you pass an inner exception into it
#define THROW_NEW_GUTIL_EXCEPTION3( ex_type, message, inner_except ) \
do{ \
    ex_type except(message); \
\
    char tmp_except_line[15];  \
    sprintf(tmp_except_line, "%d", __LINE__); \
    except.SetData("line", tmp_except_line); \
    except.SetData("file", __FILE__); \
    except.SetInnerException(inner_except); \
    throw except; \
}while(0)


GUTIL_BEGIN_NAMESPACE( Core );


// The base class for all of my exceptions
class Exception :
        public std::exception
{
public:

    Exception(const std::string &message = "");
    Exception(const Exception &);
    virtual ~Exception() throw();

    PROPERTY( Message, std::string );

    void SetData(const std::string &, const std::string &);
    std::string GetData(const std::string &) const;

    // Get a list of the keys you've put in the data collection
    std::vector<std::string> GetDataKeys(bool include_blanks = false) const;

    std::string ToString() const;

    inline void SetInnerException(const Exception &ex){
        if(_inner_exception)
            delete _inner_exception;
        _inner_exception = new Exception(ex);
    }

    inline Exception *GetInnerException() const{ return _inner_exception; }


protected:

    Exception(const std::string &identifier, const std::string &message);


private:

    std::string _exception_id;
    std::map<std::string, std::string> _data;

    Exception *_inner_exception;

};



#define STRINGIFY( something )  #something

// Use this to declare any new exceptions
#define EXCEPTION_DECLARE( ex_name ) \
class ex_name##Exception : public GUtil::Core::Exception \
{ \
public: \
    ex_name##Exception(const std::string &message = "") \
        :Exception(STRINGIFY(ex_name), message ){} \
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


GUTIL_END_NAMESPACE

#endif // GEXEPTION_H
