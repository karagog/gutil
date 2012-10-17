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

#include "extendedexception.h"
#include <new>
USING_NAMESPACE_GUTIL1(DataObjects);

NAMESPACE_GUTIL


ExtendedException::ExtendedException(const Exception<> &inner_exception)
{
    SetInnerException(inner_exception);
}

ExtendedException::~ExtendedException(){}

ExtendedException::ExtendedException(const ExtendedException &o)
    :_data(o._data)
{
    if(o.GetInnerException())
        SetInnerException(*o.GetInnerException());
}

ExtendedException &ExtendedException::operator = (const ExtendedException &o)
{
    this->~ExtendedException();
    new(this) ExtendedException(o);
    return *this;
}

void ExtendedException::SetInnerException(const Exception<false> &ex)
{
    m_innerException.Clear();

    Exception<true> const *extended( dynamic_cast<Exception<true> const *>(&ex) );
    if(extended)
        m_innerException = new Exception<true>(*extended);
    else
        m_innerException = new Exception<false>(ex);
}


END_NAMESPACE_GUTIL
