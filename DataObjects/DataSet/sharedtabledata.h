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

#ifndef SHAREDTABLEDATA_H
#define SHAREDTABLEDATA_H

#include "DataObjects/DataSet/datacolumncollection.h"
#include "DataObjects/DataSet/datarowcollection.h"
#include "Custom/gshareddata.h"
#include "Core/Interfaces/iupdatable.h"
#include <QSet>

GUTIL_BEGIN_NAMESPACE( DataObjects );


class DataSet;

class SharedTableData :
        public Custom::GSharedData,
        public Core::Interfaces::IUpdatable
{
    friend class DataTable;

public:

    SharedTableData();
    SharedTableData(const DataSet &ds);
    SharedTableData(const SharedTableData &d);

    inline DataSet &GetDataSet(){
        if(!_dataset)
            THROW_NEW_GUTIL_EXCEPTION(Core::NullReferenceException,
                                      "Table '%1' has no parent data set");
        return *_dataset;
    }

    inline const DataSet &GetDataSet() const{
        if(!_dataset)
            THROW_NEW_GUTIL_EXCEPTION(Core::NullReferenceException,
                                      "Table '%1' has no parent data set");
        return *_dataset;
    }

    void SetDataSet(const DataSet &ds);

    inline const DataRowCollection &Rows() const{
        return _rows;
    }

    inline DataRowCollection &Rows(){
        return _rows;
    }

    inline DataColumnCollection &Columns(){
        return _columns;
    }

    inline const DataColumnCollection &Columns() const{
        return _columns;
    }


    inline QString GetName() const{
        return _name;
    }

    inline void SetName(const QString &v){
        _name = v;
    }

    inline QSet<int> &KeyColumns(){
        return _key_columns;
    }

    inline const QSet<int> &KeyColumns() const{
        return _key_columns;
    }


private:

    DataSet *_dataset;
    DataRowCollection _rows;
    DataColumnCollection _columns;
    QString _name;
    QSet<int> _key_columns;

};


GUTIL_END_NAMESPACE

#endif // SHAREDTABLEDATA_H
