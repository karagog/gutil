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

#include "explicitlysharedobject.h"

GUTIL_USING_NAMESPACE( DataObjects );
GUTIL_USING_NAMESPACE( Custom );

ExplicitlySharedObject::ExplicitlySharedObject(GSharedData *d)
    :_explicitly_shared_data(d){}

ExplicitlySharedObject::ExplicitlySharedObject(const ExplicitlySharedObject &o)
    :_explicitly_shared_data(o._explicitly_shared_data){}

ExplicitlySharedObject::~ExplicitlySharedObject(){}

GSharedData *ExplicitlySharedObject::GetExplicitlySharedData() const
{
    return _explicitly_shared_data.data();
}

void ExplicitlySharedObject::SetExplicitlySharedData(GSharedData *d)
{
    _explicitly_shared_data = d;
}
