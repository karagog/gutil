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

#include "flattreemodel.h"
#include "gutil_macros.h"

NAMESPACE_GUTIL2(QT, DataModels);


void FlatTreeModel::setSourceModel(QAbstractItemModel *m)
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

    refreshSourceModel();
}

void FlatTreeModel::source_model_reset()
{
    // This is an expensive operation, so we only do it once, storing
    //  the results we calculate for quick lookup while we refresh the indexes
    _refresh_child_record();

    _index_map_to_source.Clear();
    _index_map_from_source.Clear();
    _refresh_index_mapping();

    // To save memory we'll clear this map 'cause we don't use it after here
    _child_record.Clear();

    endResetModel();
}

void FlatTreeModel::_refresh_child_record()
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

int FlatTreeModel::_count_preceeding_indexes(const QModelIndex &ind) const
{
    if(!ind.isValid())
        return 0;

    QAbstractItemModel *m = sourceModel();
    int ret = 1;

    // Ascend the tree hierarchy towards the top and count all the indexes
    //  before me but still under the same parent
    for(int i = ind.row() - 1; i >= 0; i--)
    {
        ret += _child_record.At(m->index(i, 0, ind.parent()).internalId());
    }

    // Then add in my parent's preceeding indeces
    ret += _count_preceeding_indexes(ind.parent());

    return ret;
}

int FlatTreeModel::_count_child_indexes(const QModelIndex &ind)
{
    // Start with one to account for 'ind'
    int ret = 1;

    int children = sourceModel()->rowCount(ind);
    for(int i = 0; i < children; i++)
    {
        ret += _count_child_indexes(sourceModel()->index(i, 0, ind));
    }

    // Remember this result for later
    _child_record.Insert(ind.internalId(), ret);

    return ret;
}

void FlatTreeModel::_refresh_index_mapping(const QModelIndex &ind)
{
    QAbstractItemModel *sm = sourceModel();
    if(sm)
    {
        for(int i = sm->rowCount(ind) - 1; i >= 0; i--)
        {
            QModelIndex targ = sm->index(i, 0, ind);
            QModelIndex proxy_index = _map_from_source(targ);

            _index_map_to_source.Insert(proxy_index.internalId(), QPersistentModelIndex(targ));
            _index_map_from_source.Insert(targ.internalId(), proxy_index.row());

            _refresh_index_mapping(targ);
        }
    }
}


END_NAMESPACE_GUTIL2;
