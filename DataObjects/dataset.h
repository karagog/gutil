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

#ifndef DATASET_H
#define DATASET_H

#include "datatable.h"
#include "explicitlysharedobject.h"
#include "Custom/gshareddatapointer.h"
#include "Interfaces/iqxmlserializable.h"
#include "Core/Interfaces/ireadonlyobject.h"
#include "Core/Interfaces/iupdatable.h"
#include "Core/Interfaces/iequatable.h"
#include "Core/Interfaces/iclonable.h"

#define TABLE_ACCESSOR( name, type, index ) \
            const type &Get##name##Table() const{ return (const type &)Tables()[index]; } \
            type &Get##name##Table(){ return (type &)Tables()[index]; } \
            enum{}

GUTIL_BEGIN_NAMESPACE( DataObjects );


class SharedSetData;
class DataTableCollection;

class DataSet :
        public ExplicitlySharedObject<SharedSetData>,
        public Interfaces::IQXmlSerializable,
        public Core::Interfaces::IReadOnlyObject,
        public Core::Interfaces::IUpdatable,
        public Core::Interfaces::IEquatable<DataSet>,
        public Core::Interfaces::IClonable<DataSet>
{
    template<class T> friend class DataTableBase;
    template<class T> friend class SharedTableData;
    friend class DataTableCollection;

public:

    DataSet(int num_tables = 0);
    DataSet(const DataSet &);
    virtual ~DataSet();

    const DataTableCollection &Tables() const;
    DataTableCollection &Tables();
    int TableCount() const;

    DataTable &AddTable(const DataTable &);

    bool IsNull() const{ return TableCount() == 0; }
    void Clear();

    DataSet &operator =(const DataSet &);
    DataTable &operator [](int);
    const DataTable &operator [](int) const;
    DataTable &operator [](const QString &table_name);
    const DataTable &operator [](const QString &table_name) const;

    bool Contains(const DataTable &) const;
    int GetTableIndex(const QString &table_name) const;

    DataSet Clone() const;


    // IEquatable interface:
    virtual bool Equals(const DataSet &) const;

    // IQXmlSerializable
    void WriteXml(QXmlStreamWriter &) const;
    void ReadXml(QXmlStreamReader &)
            throw(GUtil::Core::XmlException);


protected:

    DataSet(SharedSetData *);

    const SharedSetData &set_data() const;
    SharedSetData &set_data();

    // IUpdatable interface:
    virtual void commit_reject_changes(bool commit);

    // IClonable interface:
    virtual DataSet &CloneTo(DataSet &) const;

    // Derived classes take advantage of these
    virtual void write_xml_protected(QXmlStreamWriter &) const{}
    virtual void read_xml_protected(QXmlStreamReader &){}

};


GUTIL_END_NAMESPACE


// Here we add headers which we don't depend on, but will make our lives easier
//  while using this class so we don't have to remember to include these
#include "datatablecollection.h"
#include "datacolumncollection.h"

#endif // DATASET_H
