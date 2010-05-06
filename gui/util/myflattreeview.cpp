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

#include "myflattreeview.h"

using namespace GUtil::QtUtil;

myFlatTreeModel::myFlatTreeModel(QObject *parent) :
    QAbstractProxyModel(parent)
{
}

void myFlatTreeModel::setSourceModel(QAbstractItemModel *m)
{
    QAbstractProxyModel::setSourceModel(m);

    if(m)
    {
        // So we can reset ourself when something changes
        Q_ASSERT(connect(m, SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)),
                this, SLOT(source_model_rows_about_to_be_inserted(QModelIndex,int,int))));
        Q_ASSERT(connect(m, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)),
                this, SLOT(source_model_rows_about_to_be_removed(QModelIndex,int,int))));

        Q_ASSERT(connect(m, SIGNAL(rowsInserted(QModelIndex, int, int)),
                this, SLOT(source_model_rows_inserted())));
        Q_ASSERT(connect(m, SIGNAL(rowsRemoved(QModelIndex, int, int)),
                this, SLOT(source_model_rows_removed())));

        Q_ASSERT(connect(m, SIGNAL(rowsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)),
                this, SLOT(source_model_rows_about_to_be_moved(QModelIndex,int,int, QModelIndex, int))));
        Q_ASSERT(connect(m, SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)),
                this, SLOT(source_model_rows_moved())));

        Q_ASSERT(connect(m, SIGNAL(modelAboutToBeReset()), this, SLOT(source_model_about_to_be_reset())));
        Q_ASSERT(connect(m, SIGNAL(modelReset()), this, SLOT(source_model_reset())));
    }
    else
    {
        disconnect(this, SLOT(source_model_rows_about_to_be_inserted(QModelIndex,int,int)));
        disconnect(this, SLOT(source_model_rows_about_to_be_removed(QModelIndex,int,int)));
        disconnect(this, SLOT(source_model_rows_inserted(QModelIndex,int,int)));
        disconnect(this, SLOT(source_model_rows_removed(QModelIndex,int,int)));
        disconnect(this, SLOT(source_model_rows_about_to_be_moved(QModelIndex,int,int,QModelIndex,int)));
        disconnect(this, SLOT(source_model_rows_moved()));
        disconnect(this, SLOT(_reset_model()));
    }

    _reset_model();
}

void myFlatTreeModel::source_model_rows_about_to_be_inserted(const QModelIndex &ind,
                                                            int start, int end)
{
    int proxy_row = mapFromSource(ind).row();
    beginInsertRows(QModelIndex(), proxy_row + start, proxy_row + end);
}

void myFlatTreeModel::source_model_rows_about_to_be_removed(const QModelIndex &ind,
                                                           int start, int end)
{
    int proxy_row = mapFromSource(ind).row();
    beginRemoveRows(QModelIndex(), proxy_row + start, proxy_row + end);
}

void myFlatTreeModel::source_model_rows_about_to_be_moved(const QModelIndex &ind,
                                                         int start, int end,
                                                         const QModelIndex &targ,
                                                         int dest)
{
    int proxy_row1 = mapFromSource(ind).row();
    int proxy_row2 = mapFromSource(targ).row();
    beginMoveRows(QModelIndex(), proxy_row1 + start, proxy_row1 + end,
                  QModelIndex(), proxy_row2 + dest);
}

void myFlatTreeModel::source_model_rows_inserted()
{
    endInsertRows();
}

void myFlatTreeModel::source_model_rows_removed()
{
    endRemoveRows();
}

void myFlatTreeModel::source_model_rows_moved()
{
    endMoveRows();
}

void myFlatTreeModel::source_model_about_to_be_reset()
{
    beginResetModel();
}

void myFlatTreeModel::source_model_reset()
{
    _refresh_child_record();
    reset();
    endResetModel();
}

void myFlatTreeModel::_reset_model()
{
    beginResetModel();

    // This is an expensive operation, so we only do it when we have to, storing
    //  the results we calculate for quick lookup later
    _refresh_child_record();

    reset();
    endResetModel();
}

void myFlatTreeModel::_refresh_child_record()
{
    _child_record.clear();
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
    QAbstractItemModel *sm = sourceModel();
    int targetrow = proxyIndex.row();

    QModelIndex tmppar;
    int count = 0;
    int lim = sm->rowCount(tmppar);
    int i;

    for(i = 0; i < lim; i++)
    {
        if(count == targetrow)
            break;

        QModelIndex targ = sm->index(i, 0, tmppar);

        int tmpsum = count + _child_record[targ.internalId()];
        if(tmpsum > targetrow)
        {
            tmppar = targ;

            // Reset the loop as we descend into this index
            i = -1;
            lim = sm->rowCount(tmppar);

            // count the one that we're descending into
            count++;
            continue;
        }

        count = tmpsum;
    }

    return sm->index(i, 0, tmppar);
}

QModelIndex myFlatTreeModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if(sourceModel())
        return index(_count_preceeding_indexes(sourceIndex) - 1, 0);
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
