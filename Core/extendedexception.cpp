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

#include "extendedexception.h"
GUTIL_USING_NAMESPACE(Core);
GUTIL_USING_CORE_NAMESPACE(DataObjects);

ExtendedException::ExtendedException()
    :_inner_exception(0)
{}

ExtendedException::ExtendedException(const String &message,
                                     const Exception<> *inner_exception)
    :_p_Message(message),
      _inner_exception(0)
{
    if(inner_exception)
        SetInnerException(*inner_exception);
}

ExtendedException::~ExtendedException()
{
    if(_inner_exception)
        delete _inner_exception;
}

ExtendedException::ExtendedException(const ExtendedException &o)
    :_p_Message(o._p_Message),
      _data(o._data),
      _inner_exception(0)
{
    if(o.GetInnerException())
        SetInnerException(*o.GetInnerException());
}

ExtendedException &ExtendedException::operator = (const ExtendedException &o)
{
    _p_Message = o._p_Message;
    _data = o._data;
    if(o.GetInnerException())
        SetInnerException(*o.GetInnerException());
    return *this;
}

void ExtendedException::SetInnerException(const Exception<> &ex)
{
    if(_inner_exception)
        delete _inner_exception;

    const ExtendedException *extended( dynamic_cast<const ExtendedException *>(&ex) );
    if(extended)
    {
        _inner_exception = new Exception<true>(ex, *extended);
    }
    else
    {
        _inner_exception = new Exception<false>(ex);
    }
}
