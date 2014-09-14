/*Copyright 2010-2013 George Karagoulis

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

NAMESPACE_GUTIL;


ExtendedException::~ExtendedException(){}

ExtendedException::ExtendedException(const ExtendedException &o)
    :Data(o.Data),
      m_innerException(o.GetInnerException() ? o.GetInnerException()->Clone() : NULL)
{}

ExtendedException &ExtendedException::operator = (const ExtendedException &o)
{
    this->~ExtendedException();
    new(this) ExtendedException(o);
    return *this;
}


END_NAMESPACE_GUTIL;
