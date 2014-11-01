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

#ifndef CORE_IXMLSERIALIZABLE_H
#define CORE_IXMLSERIALIZABLE_H

#include <gutil/macros.h>
#include <gutil/exception.h>
#include <gutil/strings.h>

NAMESPACE_GUTIL;


/** An abstract interface which says your class can be serialized into
    an XML string.
*/
class IXmlSerializable
{
public:

    /** Serialize your class into an XML string. */
    virtual String ToXmlString(bool human_readable) const = 0;

    /** Construct your class from an existing XML string. */
    virtual void FromXmlString(const String &) = 0;

};


END_NAMESPACE_GUTIL;

#endif // CORE_IXMLSERIALIZABLE_H
