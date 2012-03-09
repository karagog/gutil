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

#ifndef GUTIL_BITVECTOR_H
#define GUTIL_BITVECTOR_H

#include "gutil_vector.h"

NAMESPACE_GUTIL1(DataObjects);


/** \file

    Declares a template specialization of Vector<bool>, and defines the
    BitVector typedef of said class
*/


/** A template specialization of Vector<bool>, which efficiently implements
    a vector of bits.

    A vector of bits is more-or-less an integer, where each individual bit
    represents a binary value in the array.  When the array size grows past the
    size of one integer, we allocate more integers in a contiguous array.

    This class' code is built into the core library, rather than left in a header file.
    This improves build time and code space.

    \note This is defined outside of the normal Vector class file, which
    is intentional.  You have to include the bitvector header to benefit from
    the improved implementation, and then you can refer to it as a BitVector
*/
template<>
class Vector<bool> :
        private Vector<GINT32>
{
public:

    inline Vector(){}
    Vector(GUINT32 capacity);
    Vector(GUINT32 size, bool init_val);

    /** This enum doesn't exist in the normal Vector implementation,
        so your code can check this at compile time to make absolute sure
        you're taking advantage of the boolean vector specialization.

        \note Put this in an assert statement rather than checking it in your
        final code.  This is really only useful for debugging.
        Note also that if the symbol BitVector is available, then the correct
        file must have already been included, so if the compiler doesn't complain
        about BitVector, you should be good to go.
    */
    enum{ IS_SPECIALIZED_BITVECTOR = 1 };

};


/** With this you can refer to Vector<bool> as a BitVector */
typedef Vector<bool> BitVector;


END_NAMESPACE_GUTIL1;

#endif // GUTIL_BITVECTOR_H
