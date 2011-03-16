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

#include "gtreeview.h"
#include "gutil_macros.h"
GUTIL_USING_NAMESPACE(Custom);

GTreeView::GTreeView(QWidget *parent)
    :QTreeView(parent)
{}

void GTreeView::ExpandToIndex(QModelIndex ind)
{
    QModelIndex par( ind.parent() );

    if(par.isValid())
    {
        // Expand my parent's parent before expanding my parent
        ExpandToIndex(par);

        // Then expand my parent
        expand(par);
    }
}

void GTreeView::CollapseWithParents(QModelIndex ind)
{
    if(ind.isValid())
    {
        collapse(ind);
        CollapseWithParents(ind.parent());
    }
}

void GTreeView::SelectRow(QModelIndex r)
{
    selectionModel()->select(r,
                             QItemSelectionModel::Rows |
                             QItemSelectionModel::ClearAndSelect);
    selectionModel()->setCurrentIndex(r, QItemSelectionModel::Select);
}

void GTreeView::SelectChildrenOfIndex(QModelIndex ind)
{
    SelectChildrenOfIndex(ind,
                     QItemSelectionModel::Select |
                     QItemSelectionModel::Rows);
}

void GTreeView::SelectChildrenOfIndex(QModelIndex ind, QItemSelectionModel::SelectionFlags fl)
{
    QItemSelection sel;
    if(ind.isValid())
        ind = model()->index(ind.row(), 0, ind.parent());
    _append_children(ind, sel, fl);
    selectionModel()->select(sel, fl);
}

void GTreeView::_append_children(const QModelIndex &ind, QItemSelection &sel, QItemSelectionModel::SelectionFlags fl)
{
    if(model()->canFetchMore(ind))
        model()->fetchMore(ind);

    sel.merge(QItemSelection(model()->index(0, 0, ind),
                             model()->index(model()->rowCount(ind) - 1, 0, ind)), fl);

    for(int i = 0; i < model()->rowCount(ind); i++)
        _append_children(model()->index(i, 0, ind), sel, fl);
}
