/*Copyright 2012-2015 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_NO_GUI_FUNCTIONALITY


#include "treeview.h"
#include <gutil/macros.h>
#include <QContextMenuEvent>
#include <QHeaderView>

NAMESPACE_GUTIL1(Qt);


void TreeView::ExpandToIndex(const QModelIndex &ind)
{
    if(ind.parent().isValid()){
        // Expand my parent's parent before expanding my parent
        ExpandToIndex(ind.parent());
        expand(ind.parent());
    }
}

void TreeView::CollapseWithParents(const QModelIndex &ind)
{
    if(ind.isValid()){
        collapse(ind);
        CollapseWithParents(ind.parent());
    }
}

void TreeView::SelectChildrenOfIndex(QModelIndex ind, int selection_flags)
{
    QItemSelection sel;
    if(ind.isValid())
        ind = model()->index(ind.row(), 0, ind.parent());
    _append_children_to_selection(ind, sel, selection_flags);
    selectionModel()->select(sel, QItemSelectionModel::SelectionFlags(selection_flags));
}


void TreeView::_append_children_to_selection(const QModelIndex &ind, QItemSelection &sel, int fl)
{
    while(model()->canFetchMore(ind))
        model()->fetchMore(ind);

    sel.merge(QItemSelection(model()->index(0, 0, ind),
                             model()->index(model()->rowCount(ind) - 1, 0, ind)),
              QItemSelectionModel::SelectionFlags(fl));

    for(int i = 0; i < model()->rowCount(ind); i++)
        _append_children_to_selection(model()->index(i, 0, ind), sel, fl);
}

void TreeView::ResizeColumnsToContents()
{
    header()->resizeSections(QHeaderView::ResizeToContents);
}

END_NAMESPACE_GUTIL1;


#endif // GUI_FUNCTIONALITY
