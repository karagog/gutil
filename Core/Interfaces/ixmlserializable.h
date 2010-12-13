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

#ifndef CORE_IXMLSERIALIZABLE_H
#define CORE_IXMLSERIALIZABLE_H

#include "gutil_macros.h"
#include "Core/exception.h"
#include <string>

GUTIL_BEGIN_CORE_NAMESPACE( Interfaces );


class IXmlSerializable
{
public:
    virtual std::string ToXmlString(bool human_readable) const = 0;
    virtual void FromXmlString(const std::string &)
            throw(GUtil::Core::XmlException) = 0;

    virtual ~IXmlSerializable(){}
};


GUTIL_END_CORE_NAMESPACE

#endif // CORE_IXMLSERIALIZABLE_H
