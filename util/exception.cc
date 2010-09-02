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

Exception::Exception()
    :exception()
{

}

Exception::Exception(const std::string & message)
    :exception()
{
    _message = message;
}

std::string Exception::Message() const
{
    return _message;
}

void Exception::SetMessage(const std::string &msg)
{
    _message = msg;
}





NotImplementedException::NotImplementedException()
    :Exception()
{

}

NotImplementedException::NotImplementedException(const std::string &message)
    :Exception(message)
{

}
