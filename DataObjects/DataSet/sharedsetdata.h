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

#ifndef SHAREDSETDATA_H
#define SHAREDSETDATA_H

#include "DataObjects/gvariantcollection.h"
#include "DataObjects/DataSet/datarowcollection.h"
#include "DataObjects/DataSet/datatable.h"
#include "DataObjects/DataSet/datatablecollection.h"
#include "Custom/gshareddata.h"
#include <QSharedData>
#include <QString>
#include <QVariantList>

GUTIL_BEGIN_NAMESPACE( DataObjects );


class DataSet;
class DataColumnCollection;
class DataRow;
class DataTableCollection;

class SharedSetData :
        public Custom::GSharedData
{
public:

    SharedSetData(int num_tables = 0);
    SharedSetData(const SharedSetData &);
    virtual ~SharedSetData();

    DataTableCollection &Tables();
    const DataTableCollection &Tables() const;


private:

    DataTableCollection *_tables;

};


GUTIL_END_NAMESPACE

#endif // SHAREDSETDATA_H
