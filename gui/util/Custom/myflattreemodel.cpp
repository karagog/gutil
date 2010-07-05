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

#include "myflattreemodel.h"

using namespace GUtil::QtUtil::Custom;

myFlatTreeModel::myFlatTreeModel(QObject *parent) :
    QAbstractProxyModel(parent)
{
}

void myFlatTreeModel::setSourceModel(QAbstractItemModel *m)
{
    QAbstractItemModel *curmodel = sourceModel();
    if(curmodel == m)
    {
        refreshSourceModel();
        return;
    }

    if(curmodel)
    {
        curmodel->disconnect(this, SLOT(source_model_about_to_be_reset()));
        curmodel->disconnect(this, SLOT(source_model_reset()));
        curmodel->disconnect(this, SLOT(source_model_data_changed(QModelIndex,QModelIndex)));
    }

    QAbstractProxyModel::setSourceModel(m);

    if(m)
    {
        // So we can reset ourself when something changes
        connect(m, SIGNAL(modelAboutToBeReset()), this, SLOT(source_model_about_to_be_reset()));
        connect(m, SIGNAL(modelReset()), this, SLOT(source_model_reset()));

        connect(m, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)),
                this, SLOT(source_model_about_to_be_reset()));
        connect(m, SIGNAL(rowsInserted(QModelIndex, int, int)),
                this, SLOT(source_model_reset()));

        connect(m, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
                this, SLOT(source_model_about_to_be_reset()));
        connect(m, SIGNAL(rowsRemoved(QModelIndex, int, int)),
                this, SLOT(source_model_reset()));

        connect(m, SIGNAL(rowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)),
                this, SLOT(source_model_about_to_be_reset()));
        connect(m, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
                this, SLOT(source_model_reset()));

        connect(m, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                this, SLOT(source_model_data_changed(QModelIndex,QModelIndex)));
    }

    _reset_model();
}

void myFlatTreeModel::source_model_data_changed(const QModelIndex &ind1, const QModelIndex &ind2)
{
    emit dataChanged(mapFromSource(ind1), mapFromSource(ind2));
}

void myFlatTreeModel::source_model_about_to_be_reset()
{
    beginResetModel();
}

void myFlatTreeModel::source_model_reset()
{
    // This is an expensive operation, so we only do it once, storing
    //  the results we calculate for quick lookup while we refresh the indexes
    _refresh_child_record();

    _index_map_to_source.clear();
    _index_map_from_source.clear();
    _refresh_index_mapping();

    // To save memory we'll clear this map 'cause we don't use it after here
    _child_record.clear();

    endResetModel();
}

void myFlatTreeModel::refreshSourceModel()
{
    _reset_model();
}

void myFlatTreeModel::_reset_model()
{
    beginResetModel();
    source_model_reset();
}

void myFlatTreeModel::_refresh_child_record()
{
    _total_rows = 0;

    QAbstractItemModel *m = sourceModel();
    if(m)
    {
        for(int i = sourceModel()->rowCount() - 1; i >= 0; i--)
        {
            _total_rows += _count_child_indexes(m->index(i, 0));
        }
    }
}

QModelIndex myFlatTreeModel::mapToSource(const QModelIndex &proxyIndex) const
{
    return _index_map_to_source[proxyIndex.internalId()];
}

QModelIndex myFlatTreeModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    int t = _index_map_from_source[sourceIndex.internalId()];
    if(t == 0 || !sourceIndex.isValid())
        return QModelIndex();
    return index(t, sourceIndex.column());
}

QModelIndex myFlatTreeModel::_map_from_source(const QModelIndex &sourceIndex)
{
    if(sourceModel())
        return index(_count_preceeding_indexes(sourceIndex) - 1, sourceIndex.column());
    return QModelIndex();
}

int myFlatTreeModel::_count_preceeding_indexes(const QModelIndex &ind) const
{
    if(!ind.isValid())
        return 0;

    QAbstractItemModel *m = sourceModel();
    int ret = 1;

    // Ascend the tree hierarchy towards the top and count all the indexes
    //  before me but still under the same parent
    for(int i = ind.row() - 1; i >= 0; i--)
    {
        ret += _child_record[m->index(i, 0, ind.parent()).internalId()];
    }

    // Then add in my parent's preceeding indeces
    ret += _count_preceeding_indexes(ind.parent());

    return ret;
}

int myFlatTreeModel::_count_child_indexes(const QModelIndex &ind)
{
    // Start with one to account for 'ind'
    int ret = 1;

    QAbstractItemModel *m = sourceModel();
    int children = m->rowCount(ind);
    for(int i = 0; i < children; i++)
    {
        ret += _count_child_indexes(m->index(i, 0, ind));
    }

    // Remember this result for later
    _child_record.insert(ind.internalId(), ret);

    return ret;
}

void myFlatTreeModel::_refresh_index_mapping(const QModelIndex &ind)
{
    QAbstractItemModel *sm = sourceModel();
    if(sm)
    {
        for(int i = sm->rowCount(ind) - 1; i >= 0; i--)
        {
            QModelIndex targ = sm->index(i, 0, ind);
            QModelIndex proxy_index = _map_from_source(targ);

            _index_map_to_source.insert(proxy_index.internalId(), QPersistentModelIndex(targ));
            _index_map_from_source.insert(targ.internalId(), proxy_index.row());

            _refresh_index_mapping(targ);
        }
    }
}

QModelIndex myFlatTreeModel::parent(const QModelIndex &) const
{
    // We're a flat table
    return QModelIndex();
}

QModelIndex myFlatTreeModel::index(int row, int column, const QModelIndex &par) const
{
    if(par.isValid())
        return QModelIndex();

    // Use the row as a unique id
    return createIndex(row, column, row);
}

int myFlatTreeModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return _total_rows;
}

int myFlatTreeModel::columnCount(const QModelIndex &parent) const
{
    QAbstractItemModel *m = sourceModel();
    if(m)
        return m->columnCount(mapToSource(parent));
    return 0;
}

QVariant myFlatTreeModel::data(const QModelIndex &proxyIndex, int role) const
{
    return mapToSource(proxyIndex).data(role);
}
