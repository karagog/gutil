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

#include "exception.h"
GUTIL_USING_NAMESPACE(Core);

Exception::Exception(const std::string &message)
    :exception(),
      _p_Message(message),
      _inner_exception(0)
{}

Exception::Exception(const std::string &id,
                     const std::string &message)
    :exception(),
      _p_Message(message),
      _exception_id(id),
      _inner_exception(0)
{}

Exception::Exception(const Core::Exception &o)
    :exception(o),
      _p_Message(o._p_Message),
      _exception_id(o._exception_id),
      _data(o._data),
      _inner_exception(o._inner_exception ?
                           new Exception(*o._inner_exception) : 0)
{}

Exception::~Exception() throw()
{
    if(_inner_exception)
        delete _inner_exception;
}

void Exception::SetData(const std::string &key, const std::string &value)
{
    _data[key] = value;
}

std::string Exception::GetData(const std::string &key) const
{
    return _data.at(key);
}

std::vector<std::string> Exception::GetDataKeys(bool include_blanks) const
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

std::string Exception::ToString() const
{
    return "GUtil::Core::" + _exception_id + "Exception";
}
