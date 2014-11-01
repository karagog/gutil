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

#include "datatablemodel.h"
USING_NAMESPACE_GUTIL;

NAMESPACE_GUTIL1(Qt);


DataTableModel::DataTableModel(QObject *parent)
    :QAbstractTableModel(parent),
      _p_DataTable(0)
{}

int DataTableModel::rowCount(const QModelIndex &parent) const
{
    if(!_p_DataTable ||
            parent.isValid())
        return 0;

    return GetDataTable()->RowCount();
}

int DataTableModel::columnCount(const QModelIndex &parent) const
{
    if(!_p_DataTable ||
            parent.isValid())
        return 0;

    return GetDataTable()->ColumnCount();
}

QVariant DataTableModel::data(const QModelIndex &index, int role) const
{
    if(!_p_DataTable ||
            !index.isValid())
        return QVariant();

    return _data(index, role);
}

QVariant DataTableModel::_data(const QModelIndex &index, int role) const
{
    QVariant ret;

    switch((::Qt::ItemDataRole)role)
    {
    case ::Qt::DisplayRole:
        ret = (*GetDataTable())[index.row()][index.column()];
        break;
    case ::Qt::EditRole:
        ret = (*GetDataTable())[index.row()][index.column()];
        break;
    default:
        break;
    }

    return ret;
}

QVariant DataTableModel::headerData(int section, ::Qt::Orientation orientation, int role) const
{
    if(!_p_DataTable)
        return QVariant();

    return _headerData(section, orientation, role);
}

QVariant DataTableModel::_headerData(int section, ::Qt::Orientation orientation, int role) const
{
    QVariant ret;

    if(orientation == ::Qt::Horizontal)
    {
        switch((::Qt::ItemDataRole)role)
        {
        case ::Qt::DisplayRole:
            ret = GetDataTable()->Columns()[section].Value().GetName().ToQString();
            break;
        default:
            break;
        }
    }
    else if(orientation == ::Qt::Vertical)
    {
        switch((::Qt::ItemDataRole)role)
        {
        case ::Qt::DisplayRole:
            ret = section;
            break;
        default:
            break;
        }
    }

    return ret;
}

bool DataTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!_p_DataTable || !index.isValid())
        return false;

    return _setData(index, value, role);
}

bool DataTableModel::_setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ret(true);

    switch((::Qt::ItemDataRole)role)
    {
    case ::Qt::DisplayRole:
        (*GetDataTable())[index.row()][index.column()] = value;
        break;
    case ::Qt::EditRole:
        (*GetDataTable())[index.row()][index.column()] = value;
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}

::Qt::ItemFlags DataTableModel::flags(const QModelIndex &index) const
{
    ::Qt::ItemFlags ret(::Qt::ItemIsEnabled |
                      ::Qt::ItemIsSelectable |
                      ::Qt::ItemIsUserCheckable);

    if(!_p_DataTable ||
            !index.isValid())
        return ret;

    ret |= ::Qt::ItemIsEditable;
    ret |= ::Qt::ItemIsDragEnabled;

    return ret;
}

bool DataTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if(!GetDataTable())
        return false;

    beginInsertRows(parent, row, row + count - 1);
    {
        for(int i = 0; i < count; i++)
        {
            GetDataTable()->InsertRow(GetDataTable()->CreateRow(), row);
        }
    }
    endInsertRows();
    return true;
}

bool DataTableModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    if(!GetDataTable())
        return false;

    beginInsertColumns(parent, column, column + count - 1);
    {
        for(int i = 0; i < count; i++)
            GetDataTable()->InsertColumn(DataColumn(QUuid::createUuid().toString().toUtf8().constData()), column);
    }
    endInsertColumns();
    return true;
}

bool DataTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if(!GetDataTable())
        return false;

    beginRemoveRows(parent, row, row + count - 1);
    {
        for(int i = 0; i < count; i++)
            GetDataTable()->Rows().Remove(row);
    }
    endRemoveRows();
    return true;
}

bool DataTableModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    if(!GetDataTable())
        return false;

    beginRemoveColumns(parent, column, column + count - 1);
    {
        for(int i = 0; i < count; i++)
            GetDataTable()->Columns().Remove(column);
    }
    endRemoveColumns();
    return true;
}


END_NAMESPACE_GUTIL1;
