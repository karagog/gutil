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

#include <gutil/datatable.h>
#include <gutil/variant.h>


class QXmlStreamWriter;
class QXmlStreamReader;

NAMESPACE_GUTIL1(Qt);


class VariantTable :
        public GUtil::DataTable<QVariant>,
        public IQXmlSerializable
{
public:

    VariantTable(){}
    VariantTable(int num_cols) :GUtil::DataTable<QVariant>(num_cols){}
    VariantTable(const GUtil::Vector<GUtil::String> &col_names)
        :GUtil::DataTable<QVariant>(col_names){}
    VariantTable(const GUtil::String &tbl_name, const GUtil::Vector<GUtil::String> &col_names = GUtil::Vector<GUtil::String>())
        :GUtil::DataTable<QVariant>(tbl_name, col_names)
    {}

    virtual void WriteXml(QXmlStreamWriter &) const;
    virtual void ReadXml(QXmlStreamReader &);


    static void WriteRowXml(QXmlStreamWriter &, const GUtil::DataRow<QVariant> &);
    static void ReadRowXml(QXmlStreamReader &, GUtil::DataRow<QVariant> &);

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_VARIANTTABLE_H
