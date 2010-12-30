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

#ifndef DATATABLEMODEL_H
#define DATATABLEMODEL_H

#include "gutil_macros.h"
#include "DataObjects/DataSet/datatable.h"
#include <QAbstractTableModel>


namespace GUtil{ namespace DataModels {


// Use this as a datamodel for the data table object

class DataTableModel :
        public QAbstractTableModel
{
    Q_OBJECT
public:

    explicit DataTableModel(QObject *parent = 0);
    virtual ~DataTableModel(){}

    PROPERTY_POINTER(DataTable, GUtil::DataObjects::DataTable);


    // QAbstractTableModel interface:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool insertRows(int row, int count, const QModelIndex &parent);
    bool insertColumns(int column, int count, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent);
    bool removeColumns(int column, int count, const QModelIndex &parent);


protected:

    virtual QVariant _data(const QModelIndex &, int) const;
    virtual QVariant _headerData(int, Qt::Orientation, int) const;

    virtual bool _setData(const QModelIndex &, const QVariant &, int);

};


}}

#endif // DATATABLEMODEL_H
