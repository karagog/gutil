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

#include "datarow.h"
#include "datatable.h"
#include "datatuple.h"
#include "Custom/gsemaphore.h"
using namespace GUtil;

DataObjects::DataRow::DataRow()
{
    _init_data_row(0);
}

DataObjects::DataRow::DataRow(DataObjects::DataTable *dt)
{
    _init_data_row(dt);
}

DataObjects::DataRow::DataRow(const DataRow &o)
{
    _init_data_row(0);

    *this = o;
}

DataObjects::DataRow::~DataRow()
{
    _detach_tuple();
}

void DataObjects::DataRow::_init_data_row(DataTable *dt)
{
    _tuple_semaphore = 0;

    // Initialize a blank data tuple
    _attach_tuple(0, 0);

    table = dt;

    if(table != 0)
        _tuple->Resize(table->columnCount());
}

DataObjects::DataRow &DataObjects::DataRow::operator =(const DataObjects::DataRow &o)
{
    table = o.table;
    _attach_tuple(o._tuple, o._tuple_semaphore);
    return *this;
}

void DataObjects::DataRow::_detach_tuple()
{
    _tuple_semaphore->Down();
    if(_tuple_semaphore->IsEmpty())
    {
        delete _tuple;
        delete _tuple_semaphore;
    }
}

void DataObjects::DataRow::_attach_tuple(DataTuple *t, Custom::GSemaphore *sem_other)
{
    if(t == 0)
    {
         // Are we newly initialized?
        _tuple = new DataObjects::DataTuple();
        _tuple_semaphore = new Custom::GSemaphore();
    }
    else
    {
        // Detach our current semaphore before connecting to the other's
        _detach_tuple();

        _tuple = t;
        _tuple_semaphore = sem_other;
    }

    // Up the semaphore, so that nobody deletes the tuple while we're using it
    _tuple_semaphore->Up();
}

QVariant &DataObjects::DataRow::operator [](int index)
{
    return (*_tuple)[index];
}

DataObjects::DataTable &DataObjects::DataRow::Table()
{
    return *table;
}

int DataObjects::DataRow::Index() const
{
    return row_index;
}

int DataObjects::DataRow::ColumnCount() const
{
    return table->ColumnCount();
}

void DataObjects::DataRow::WriteXml(QXmlStreamWriter &) const
{

}

void DataObjects::DataRow::ReadXml(QXmlStreamReader &)
        throw(Core::XmlException)
{

}





DataObjects::DataRowCollection::DataRowCollection(DataTable *dt)
{
    _table = dt;
}

DataObjects::DataRowCollection::~DataRowCollection(){}

void DataObjects::DataRowCollection::onAdd(void *v, int index)
{
    DataRow *dr = (DataRow *)v;

    dr->table = _table;
    dr->row_index = index;
}
