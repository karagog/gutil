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

#ifndef GUTIL_FLAGS_H
#define GUTIL_FLAGS_H

#include "gutil_globals.h"
#include "gutil_bitvector.h"
NAMESPACE_GUTIL1(DataObjects);


/** A class to encapsulate boolean memory storage on an integer type.

    Its template takes an enum parameter, which you use to specify bit-indeces of which flags to set.

    Use GUTIL_DECLARE_FLAGS to declare operators for your enum type, which will allow you to
    | (or) them together and yield a Flags type.

    \tparam EnumType The type of the enum you wish to represent.  None of the enumerations
    can be negative.
    \tparam INT_TYPE The type of integer used to store the flags.  You can optimize the
    space consumed by the flags object and tailor it to the size of the EnumType
*/
template<class EnumType, class INT_TYPE = GUINT32>
class Flags
{
    INT_TYPE m_flags;
public:

    /** Constructs a flags object with all bits set to 0. */
    inline Flags() :m_flags(0){}

    /** This constructor initializes the entire word of data from an int. */
    inline explicit Flags(INT_TYPE init_val) :m_flags(init_val){}

    /** Constructs a flags object with all bits set to 0, except the bit represented
        by init_val, which is set to 1
    */
    inline explicit Flags(EnumType init_val) :m_flags(0){ SetFlag(init_val, true); }

    /** Sets all the bits to 0, except the given bit is set to 1 */
    inline Flags<EnumType, INT_TYPE> &operator = (EnumType val){ new(this) Flags(val); return *this; }

    /** Constructs a flags object with all bits set to init_val. */
    inline explicit Flags(bool init_val) :m_flags(GUtil::BitMask<INT_TYPE>(init_val)) {}

    /** Returns true if the flag is set. */
    inline bool TestFlag(EnumType f) const{ return m_flags & (1 << f); }

    /** Sets the flag to 1 or 0, depending on the bool parameter. */
    inline void SetFlag(EnumType f, bool b){
        INT_TYPE mask(1 << f);
        if(b)   m_flags |= mask;
        else    m_flags &= ~mask;
    }

    /** Toggles the flag; if it was 1 now it's 0, and vice versa. */
    inline void ToggleFlag(EnumType f){ SetFlag(f, !TestFlag(f)); }

    /** Returns a copy of this flags object, where the specified bit has been set to 1. */
    inline Flags<EnumType, INT_TYPE> operator | (EnumType e) const{
        Flags<EnumType, INT_TYPE> ret(*this);
        ret.SetFlag(e, true);
        return ret;
    }

    /** Sets the bit to a 1 and returns this. */
    inline Flags<EnumType, INT_TYPE> &operator |= (EnumType e){
        SetFlag(e, true);
        return *this;
    }

};

END_NAMESPACE_GUTIL1;


/** Use this for convenient flag operators for your enum type.
    \param flags_name The name of the resultant flags class.
    \param enum_type The name of the enum that you wish to turn into flags
*/
#define GUTIL_DECLARE_FLAGS(flags_name, enum_type) \
            GUTIL_DECLARE_FLAGS2(flags_name, enum_type, GUINT32)

/** Use this for convenient flag operators for your enum type.
    \param flags_name The name of the resultant flags class.
    \param enum_type The name of the enum that you wish to turn into flags
    \param int_type The type of integer you want behind the flags
*/
#define GUTIL_DECLARE_FLAGS2(flags_name, enum_type, int_type) \
    class flags_name : public GUtil::DataObjects::Flags<enum_type, int_type>{ \
    public: \
        inline flags_name(){} \
        inline explicit flags_name(int_type i) :GUtil::DataObjects::Flags<enum_type, int_type>(i){} \
        inline flags_name(const GUtil::DataObjects::Flags<enum_type, int_type> &o) :GUtil::DataObjects::Flags<enum_type, int_type>(o){} \
        inline flags_name &operator = (enum_type e){ GUtil::DataObjects::Flags<enum_type, int_type>::operator = (e); return *this; } \
    }

/** Declares all necessary operators and supplement classes to integrate
    a Flags class defined with GUTIL_DECLARE_FLAGS with the rest of your code.

    You need to declare this outside of any class/namespace scope, hence why it's
    not built into the GUTIL_DECLARE_FLAGS macro

    \param flags_name The name of the flags class.
    \param enum_type The name of the enum that you wish to turn into flags
*/
#define GUTIL_DECLARE_FLAG_OPERATORS(flags_name, enum_type) \
    static inline flags_name operator | (enum_type e1, enum_type e2){ \
        flags_name ret; \
        ret.SetFlag(e1, true); ret.SetFlag(e2, true); \
        return ret; \
    } \
    \
    namespace GUtil{ \
    template<>struct IsMovableType< flags_name >{ enum{ Value = 1 }; }; \
    }enum{}


namespace GUtil
{

/** The Flag type can be binary-moved */
template<class U>struct IsMovableType< DataObjects::Flags<U> >{ enum{ Value = 1 }; };

}


#endif // GUTIL_FLAGS_H
