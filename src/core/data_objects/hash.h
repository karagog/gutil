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

#ifndef GUTIL_HASH_H
#define GUTIL_HASH_H

#include "gutil_macros.h"

NAMESPACE_GUTIL;


/** A class to generate a hash of arbitrary data.
    It uses a LUT of random data and operates in linear time
    with the size of input.
    
    \note this is not a secure hash, just one that will give you a (probably) unique
    id from an array of bytes.
*/
class Hash
{
    GUINT32 m_hash{};
public:

    /** Adds data to the hash. */
    void AddData(byte const *, GUINT32 len);
    
    /** Returns the current hash, and resets it to 0 so you can add more data
        to compute the next hash.
    */
    inline GUINT32 Final(){ GUINT32 ret = m_hash; m_hash = 0; return ret; }

    /** Convenience function statically computes hash. */
    inline static GUINT32 ComputeHash(byte const *buf, GUINT32 len){
        Hash h; h.AddData(buf, len); return h.Final();
    }
    
};


END_NAMESPACE_GUTIL;

#endif // GUTIL_HASH_H
