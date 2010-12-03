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

#ifndef ROWVALUECOLLECTION_H
#define ROWVALUECOLLECTION_H

#include "updatablegvariantcollection.h"

GUTIL_BEGIN_NAMESPACE( DataObjects );


class SharedRowData;
class DataRow;

class RowValueCollection :
        public UpdatableGVariantCollection
{
public:

    RowValueCollection(SharedRowData *rd, int size = 0);
    RowValueCollection(const RowValueCollection &o);

    // Passes a reference to the data row instance, the index being changed, and the next/previous
    //  value, accordingly
    void SetValueAboutToChangeFunction(void (*func)(const DataRow &, int, const Custom::GVariant &));
    void SetValueChangedFunction(void (*func)(const DataRow &, int, const Custom::GVariant &));


protected:

    virtual void value_about_to_change(int index, const Custom::GVariant &newval);
    virtual void value_changed(int index, const Custom::GVariant &prevval);


private:

    void (*_value_about_to_change)(const DataRow &,
                                   int index, const Custom::GVariant &newvalue);
    void (*_value_changed)(const DataRow &,
                           int index, const Custom::GVariant &oldval);

    SharedRowData *_row_data;

};


GUTIL_END_NAMESPACE;

#endif // ROWVALUECOLLECTION_H
