/*Copyright 2010-2013 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_VARIANTTABLE_H
#define GUTIL_VARIANTTABLE_H

#include "gutil_datatable.h"
#include "gutil_variant.h"


class QXmlStreamWriter;
class QXmlStreamReader;

NAMESPACE_GUTIL2(QT, DataObjects);


class VariantTable :
        public GUtil::DataObjects::DataTable<QVariant>,
        public Interfaces::IQXmlSerializable
{
public:

    VariantTable(){}
    VariantTable(int num_cols) :GUtil::DataObjects::DataTable<QVariant>(num_cols){}
    VariantTable(const GUtil::DataObjects::Vector<GUtil::DataObjects::String> &col_names)
        :GUtil::DataObjects::DataTable<QVariant>(col_names){}
    VariantTable(const GUtil::DataObjects::String &tbl_name, const GUtil::DataObjects::Vector<GUtil::DataObjects::String> &col_names = GUtil::DataObjects::Vector<GUtil::DataObjects::String>())
        :GUtil::DataObjects::DataTable<QVariant>(tbl_name, col_names)
    {}

    virtual void WriteXml(QXmlStreamWriter &) const;
    virtual void ReadXml(QXmlStreamReader &);


    static void WriteRowXml(QXmlStreamWriter &, const GUtil::DataObjects::DataRow<QVariant> &);
    static void ReadRowXml(QXmlStreamReader &, GUtil::DataObjects::DataRow<QVariant> &);

};


END_NAMESPACE_GUTIL2;

#endif // GUTIL_VARIANTTABLE_H
