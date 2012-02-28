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
NAMESPACE_GUTIL1(DataObjects);


/** A class to encapsulate boolean memory storage on an integer type.

    Its template takes an enum parameter, which you use to specify bit-indeces of which flags to set.

    Use GUTIL_DECLARE_FLAGS to declare operators for your enum type, which will allow you to
    | (or) them together and yield a Flags type.
*/
template<class EnumType>
class Flags
{
public:

    /** Constructs a flags object with all bits set to 0. */
    inline Flags() :m_flags(0){}
    /** Constructs a flags object from an integer.  Similar to the copy constructor. */
    inline Flags(GUINT32 f) :m_flags(f){}

    /** Returns true if the flag is set. */
    inline bool TestFlag(EnumType f) const{
        return (m_flags >> f) & 1;
    }

    /** Sets the flag to 1 or 0, depending on the bool parameter. */
    inline void SetFlag(EnumType f, bool b){
        const GUINT32 mask(1 << static_cast<int>(f));
        m_flags = (m_flags & ~mask) | (-b & mask);
    }

    /** Toggles the flag; if it was 1 now it's 0, and vice versa. */
    inline void ToggleFlag(EnumType f){
        SetFlag(f, !TestFlag(f));
    }

    /** Returns the integer memory of our bits, in case the programmer wants to do their own bit operations. */
    inline GUINT32 GetWord() const{ return m_flags; }

    /** Returns a copy of this flags object, where the specified bit has been set to 1. */
    inline Flags<EnumType> operator | (EnumType e) const{
        Flags<EnumType> ret(*this);
        ret.SetFlag(e, true);
        return ret;
    }

    /** Sets the bit to a 1 and returns this. */
    inline Flags<EnumType> &operator |= (EnumType e){
        SetFlag(e, true);
        return *this;
    }


private:
    GUINT32 m_flags;
};

END_NAMESPACE_GUTIL1;


/** Use this for convenient flag operators for your enum type. */
#define GUTIL_DECLARE_FLAGS(flags_name, enum_type) \
    class flags_name : public GUtil::DataObjects::Flags<enum_type>{ \
    public: \
        inline flags_name(){} \
        inline flags_name(GUINT32 i) :GUtil::DataObjects::Flags<enum_type>(i){} \
        inline flags_name(const GUtil::DataObjects::Flags<enum_type> &o) :GUtil::DataObjects::Flags<enum_type>(o){} \
    };

/** You need to declare this outside of a class scope. */
#define GUTIL_DECLARE_FLAG_OPERATORS(flags_name, enum_type) \
    static inline flags_name operator | (enum_type e1, enum_type e2){ \
        flags_name ret; \
        ret.SetFlag(e1, true); ret.SetFlag(e2, true); \
        return ret; \
    } \
    \
    namespace GUtil{ \
    template<>struct IsMovableType< flags_name >{ enum{ Value = 1 }; }; \
    }


namespace GUtil
{

/** The Flag type can be binary-moved */
template<class U>struct IsMovableType< DataObjects::Flags<U> >{ enum{ Value = 1 }; };

}


#endif // GUTIL_FLAGS_H
