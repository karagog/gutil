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

#include "updatablegvariant.h"
GUTIL_USING_NAMESPACE( Custom );


UpdatableGVariant::UpdatableGVariant(const GVariant &gv)
    :GVariant(gv),
    _backup(*this){}

UpdatableGVariant::UpdatableGVariant(const UpdatableGVariant &ogv)
    :GVariant(ogv),
    _backup(*this){}


// We override these functions from QVariant, so we can call our callback functions
UpdatableGVariant &UpdatableGVariant::operator = (const GVariant &o){
    GVariant tmp(*this);

    value_about_to_change(*this, o);
    ((QVariant &)*this) = o;
    value_changed(tmp, *this);
    return *this;
}

void UpdatableGVariant::clear(){
    GVariant tmp(*this);

    value_about_to_change(*this, GVariant());
    QVariant::clear();
    value_changed(tmp, *this);
}

void UpdatableGVariant::convert(Type t){
    GVariant tmp1(*this);
    QVariant tmp2(tmp1);
    tmp2.convert(t);

    value_about_to_change(*this, tmp2);
    QVariant::convert(t);
    value_changed(tmp1, *this);
}

template <class U> void UpdatableGVariant::setValue(const U &value){
    GVariant tmp(*this);
    GVariant tmp2(value);

    value_about_to_change(*this, tmp2);
    QVariant::setValue(value);
    value_changed(tmp, *this);
}

void UpdatableGVariant::commit_reject_changes(bool commit)
{
    if(commit)
        _backup = *this;
    else
        *this = _backup;
}

void UpdatableGVariant::value_changed(const GVariant &o, const GVariant &t)
{
    if(o != t)
        SetDirty(true);
}

void UpdatableGVariant::value_about_to_change(const GVariant &, const GVariant &){}
