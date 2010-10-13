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

#include "exception.h"
using namespace GUtil;

Core::Exception::Exception()
    :exception()
{

}

Core::Exception::Exception(const std::string & message)
    :exception()
{
    _message = message;
}

std::string Core::Exception::Message() const
{
    return _message;
}

void Core::Exception::SetMessage(const std::string &msg)
{
    _message = msg;
}

void Core::Exception::SetData(const std::string &key, const std::string &value)
{
    _data[key] = value;
}

std::string Core::Exception::GetData(const std::string &key) const
{
    return _data.at(key);
}

std::vector<std::string> Core::Exception::GetDataKeys(bool include_blanks) const
{
    std::vector<std::string> ret;

    for(std::map<std::string, std::string>::const_iterator it = _data.begin(); it != _data.end(); it++)
    {
        std::string first = it->first;
        std::string second = it->second;

        if(include_blanks || second.length() > 0)
            ret.push_back(first);
    }

    return ret;
}

std::string Core::Exception::ToString() const
{
    return "GUtil::Core::" + ToString_protected();
}

std::string Core::Exception::ToString_protected() const
{
    return "Exception";
}





Core::NotImplementedException::NotImplementedException()
    :Exception()
{

}

Core::NotImplementedException::NotImplementedException(const std::string &message)
    :Exception(message)
{

}

std::string Core::NotImplementedException::ToString_protected() const
{
    return "NotImplementedException";
}
