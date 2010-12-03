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

#ifndef COLUMNCOLLECTION_H
#define COLUMNCOLLECTION_H

#include "DataObjects/DataSet/datacolumn.h"
#include "DataObjects/collection.h"

GUTIL_BEGIN_NAMESPACE(DataObjects);


class DataColumnCollection :
        public ResizableCollection<DataColumn>
{
public:

    DataColumnCollection(int size = 0);
    DataColumnCollection(const DataColumnCollection &);

    QString Key(int) const;
    QString Label(int) const;

    QStringList Keys() const;
    QStringList Labels() const;

    void SetKey(int, const QString &);
    void SetLabel(int, const QString &);

    bool ContainsKey(const QString &) const;


protected:

    virtual void validate_new_item(const DataColumn &) const
            throw(Core::ValidationException);

};


GUTIL_END_NAMESPACE


#endif // COLUMNCOLLECTION_H
