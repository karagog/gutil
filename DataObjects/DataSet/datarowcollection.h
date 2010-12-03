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

#ifndef DATAROWCOLLECTION_H
#define DATAROWCOLLECTION_H

#include "gutil_macros.h"
#include "DataObjects/DataSet/datatable.h"

GUTIL_BEGIN_NAMESPACE( DataObjects );


// Make your own derived version of the DataRowCollection class by following this example:
template <class U> class DataRowCollectionBase;

typedef DataRowCollectionBase<DataRow> DataRowCollection;


GUTIL_END_NAMESPACE

#endif // DATAROWCOLLECTION_H
