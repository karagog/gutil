/*Copyright Copyright 2011 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef SETDATA_H
#define SETDATA_H

#include "DataObjects/DataSet/datatablecollection.h"

GUTIL_BEGIN_NAMESPACE( DataObjects );


class SetData
{
public:

    SetData(int num_tables = 0);
    SetData(const SetData &);

    DataTableCollection Tables;

};


GUTIL_END_NAMESPACE

#endif // SETDATA_H
