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

#include <stdexcept>
#include <string>
#include <map>

namespace GUtil
{
    class Exception : public std::exception
    {
    public:
        Exception();
        Exception(const std::string & message);
        ~Exception() throw(){}

        void SetMessage(const std::string &msg);

        void SetData(const std::string &, const std::string &);
        std::string GetData(const std::string &) const;

        std::string Message() const;

    private:
        std::string _message;
        std::map<std::string, std::string> _data;
    };


    class NotImplementedException : public Exception
    {
    public:
        NotImplementedException();
        NotImplementedException(const std::string &message);
    };
}

#endif // GEXEPTION_H
