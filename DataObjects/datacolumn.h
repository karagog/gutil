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

#ifndef DATACOLUMN_H
#define DATACOLUMN_H

#include "Core/Interfaces/iequatable.h"
#include "gutil_macros.h"
#include <QPair>

GUTIL_BEGIN_NAMESPACE(DataObjects);

class DataColumnCollection;

class DataColumn :
        public Core::Interfaces::IEquatable<DataColumn>
{
    friend class DataTable;
    friend class DataColumnCollection;

public:

    DataColumn(const DataColumn &);

    QString Key() const;
    QString Label() const;

    DataColumn &operator =(const DataColumn &);

    int Index() const;

    // IEquatable:
    virtual bool Equals(const DataColumn &) const;

    PROPERTY(int, MyInt);


protected:

    DataColumn(const QString &key, DataColumnCollection *c = 0);
    DataColumn(const QString &key, const QString &label, DataColumnCollection *c = 0);

    void set_key(const QString &);
    void set_label(const QString &);


private:

    QPair<QString, QString> _data;

    DataColumnCollection * _collection;

};

GUTIL_END_NAMESPACE

#endif // DATACOLUMN_H
