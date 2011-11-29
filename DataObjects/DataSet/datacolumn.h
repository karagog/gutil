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

#ifndef DATACOLUMN_H
#define DATACOLUMN_H

#include "gutil_macros.h"
#include <QUuid>

GUTIL_BEGIN_NAMESPACE(DataObjects);

class DataColumn
{
public:

    inline DataColumn(const char *key)
        :_p_Key(key)
    {}
    inline DataColumn(const QString &key = QUuid::createUuid().toString(),
                      const QString &label = QString::null)
        :_p_Key(key),
          _p_Label(label)
    {}
    inline DataColumn(const DataColumn &o)
        :_p_Key(o._p_Key),
          _p_Label(o._p_Label)
    {}

    PROPERTY(Key, QString);
    PROPERTY(Label, QString);

    inline bool operator == (const DataColumn &o) const{
        return GetKey() == o.GetKey();
    }
    inline bool operator != (const DataColumn &o) const{
        return GetKey() != o.GetKey();
    }

    inline DataColumn &operator = (const DataColumn &o){
        SetKey(o.GetKey());
        SetLabel(o.GetLabel());
        return *this;
    }

};


GUTIL_END_NAMESPACE

#endif // DATACOLUMN_H
