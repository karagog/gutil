/*Copyright 2011 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef SHAREDROWDATA_H
#define SHAREDROWDATA_H

#include "gutil.h"
#include "rowdata.h"


GUTIL_BEGIN_NAMESPACE(DataObjects);


// Simply a row data object that can be used in an explicitly shared pointer
class SharedRowData :
        public RowData,
        public QSharedData
{
public:
    inline explicit SharedRowData(TableData *td,
                                  const Custom::GVariantVector &vals = Custom::GVariantVector())
        :RowData(td, vals)
    {}
    inline SharedRowData(const SharedRowData &o)
        :RowData(o),
          QSharedData()
    {}
};


GUTIL_END_NAMESPACE;

#endif // SHAREDROWDATA_H
