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

#include "observablegvariant.h"
GUTIL_USING_NAMESPACE( Custom );

ObservableGVariant::ObservableGVariant(const GVariant &gv)
    :UpdatableGVariant(gv),
    _observer(0)
{}

ObservableGVariant::ObservableGVariant(const ObservableGVariant &ogv)
    :UpdatableGVariant(ogv),
    _observer(0)
{}

void ObservableGVariant::SetObserver(GVariantObserver *o)
{
    _observer = o;
}

void ObservableGVariant::value_about_to_change(const GVariant &o, const GVariant &t)
{
    UpdatableGVariant::value_about_to_change(o, t);

    if(_observer)
        _observer->gvariant_value_about_to_change(o, t);
}

void ObservableGVariant::value_changed(const GVariant &o, const GVariant &t)
{
    UpdatableGVariant::value_changed(o, t);

    if(_observer)
        _observer->gvariant_value_changed(o, t);
}
