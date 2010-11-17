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

#ifndef GEXCEPTION_H
#define GEXCEPTION_H

#include "gutil_macros.h"
#include <stdlib.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

// Use these convenient macros to insert the file/line data in the exception
#define THROW_GUTIL_EXCEPTION( except ) \
do{ \
    char tmp_except_line[15];  \
    except.SetData("line", std::string(itoa(__LINE__, tmp_except_line, 10))); \
    except.SetData("file", std::string(__FILE__)); \
    throw except; \
}while(0)

// pass an exception type and the message you want it to have
#define THROW_NEW_GUTIL_EXCEPTION( ex_type, message ) \
do{ \
    ex_type except(message); \
\
    char tmp_except_line[15];  \
    except.SetData("line", std::string(itoa(__LINE__, tmp_except_line, 10))); \
    except.SetData("file", std::string(__FILE__)); \
    throw except; \
}while(0)


GUTIL_BEGIN_NAMESPACE( Core );


// The base class for all of my exceptions
class Exception : public std::exception
{
public:
    Exception(const std::string &message = "");
    virtual ~Exception() throw(){}

    void SetMessage(const std::string &msg);

    void SetData(const std::string &, const std::string &);
    std::string GetData(const std::string &) const;

    // Get a list of the keys you've put in the data collection
    std::vector<std::string> GetDataKeys(bool include_blanks = false) const;

    std::string Message() const;

    std::string ToString() const;

        protected:
    // Derived exceptions should reimplement this so their type can be shown in string format
    virtual std::string ToString_protected() const;

        private:
    std::string _message;
    std::map<std::string, std::string> _data;
};



// If a method is not yet implemented
class NotImplementedException : public Exception
{
public:
    NotImplementedException(const std::string &message = "");

protected:
    virtual std::string ToString_protected() const;
};



// If you try to write to a readonly object it might throw this
class ReadOnlyException : public Exception
{
public:
    ReadOnlyException(const std::string &message = "");

protected:
    virtual std::string ToString_protected() const;
};



// When there is an error with arguments/parameters
class ArgumentException : public Exception
{
public:
    ArgumentException(const std::string &message = "");

protected:
    virtual std::string ToString_protected() const;
};



class DataTransportException : public Exception
{
public:
    DataTransportException(const std::string &message = "");

protected:
    virtual std::string ToString_protected() const;
};



class XmlException : public Exception
{
public:
    XmlException(const std::string &message = "");

protected:
    virtual std::string ToString_protected() const;
};



class EndOfFileException : public Exception
{
public:
    EndOfFileException(const std::string &message = "");

protected:
    virtual std::string ToString_protected() const;
};



class LockException : public Exception
{
public:
    LockException(const std::string &message = "");

protected:
    virtual std::string ToString_protected() const;
};



class NullReferenceException : public Exception
{
public:
    NullReferenceException(const std::string &message = "");

protected:
    virtual std::string ToString_protected() const;
};



class IndexOutOfRangeException : public Exception
{
public:
    IndexOutOfRangeException(const std::string &message = "");

protected:
    virtual std::string ToString_protected() const;
};



class ValidationException : public Exception
{
public:
    ValidationException(const std::string &message = "");

protected:
    virtual std::string ToString_protected() const;
};


GUTIL_END_NAMESPACE

#endif // GEXEPTION_H
