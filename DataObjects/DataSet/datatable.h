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

#ifndef DATATABLE_H
#define DATATABLE_H

#include "gutil_macros.h"

GUTIL_BEGIN_NAMESPACE( DataObjects );


class DataRow;
template<class T> class DataTableBase;

typedef DataTableBase<DataRow> DataTable;


GUTIL_END_NAMESPACE

#endif // DATATABLE_H
