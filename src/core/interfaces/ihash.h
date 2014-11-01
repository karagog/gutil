/*Copyright 2014 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_IHASH_H
#define GUTIL_IHASH_H

#include <gutil/iclonable.h>
#include <memory>

NAMESPACE_GUTIL;
class IInput;
class IProgressHandler;


/** A class to define a hashing interface. 

    You can use this interface in several ways:

    - The most basic usage is by calling AddData() and Final()
    - If you have a generic IInput, you can use the AddDataFromDevice() function which
        takes care of buffering the input and also allows you to monitor progress and cancel it.
        
    All hash classes must provide a clone implementation for creating the right run-time type.
*/
class IHash : public IClonable
{
public:

    /** Returns the size of the hash digest. */
    virtual GUINT32 DigestSize() const = 0;

    /** Add data to the hash. */
    virtual void AddData(byte const *, GUINT32) = 0;
    
    /** Computes the digest and fills the buffer. */
    virtual void Final(byte *) = 0;

    /** You can be deleted by this interface. */
    virtual ~IHash(){}
    
    /** \name Convenience functions.
        Technically these functions make this not a "true" interface, but they're truly generic
        and work for all instantiations.
        \{
    */

    /** Adds data from a generic input device, in chunks, and allows you to observe
        progress and even cancel it.
    */
    void AddDataFromDevice(IInput *input, GUINT32 chunk_size = 0, IProgressHandler *ph = 0);
    
    /** \} */
};


END_NAMESPACE_GUTIL;

#endif // GUTIL_IHASH_H
