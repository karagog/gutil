/*Copyright 2010-2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GTREEVIEW_H
#define GTREEVIEW_H

#ifdef GUI_FUNCTIONALITY

#include <QTreeView>

namespace GUtil{ namespace QT{ namespace Custom{


/** This is just a customized QTreeView, with extra functions
    I find to be useful.
*/
class TreeView :
        public QTreeView
{
    Q_OBJECT
public:

    inline explicit TreeView(QWidget *parent = 0)
        :QTreeView(parent){}

    /** Selects all the children recursively using the provided selection flags */
    void SelectChildrenOfIndex(QModelIndex ind, QItemSelectionModel::SelectionFlags fl){
        QItemSelection sel;
        if(ind.isValid()) ind = model()->index(ind.row(), 0, ind.parent());
        _append_children(ind, sel, fl);
        selectionModel()->select(sel, fl);
    }


public slots:

    /** Expands all parents in the tree until this index is visible */
    void ExpandToIndex(const QModelIndex &ind){
        if(ind.parent().isValid()){
            // Expand my parent's parent before expanding my parent
            ExpandToIndex(ind.parent());
            expand(ind.parent());
        }
    }

    /** Collapses the index and all its parents recursively */
    void CollapseWithParents(const QModelIndex &ind){
        if(ind.isValid()){
            collapse(ind);
            CollapseWithParents(ind.parent());
        }
    }

    /** Clears the current selection and selects the row (current index will be
        on the row's first cell
    */
    void SelectRow(const QModelIndex &r){
        selectionModel()->select(r, QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect);
        selectionModel()->setCurrentIndex(r, QItemSelectionModel::Select);
    }

    /** Selects all the child rows recursively */
    void SelectChildrenOfIndex(const QModelIndex &ind){
        SelectChildrenOfIndex(ind,
                              QItemSelectionModel::Select |
                              QItemSelectionModel::Rows);
    }


private:

    void _append_children(const QModelIndex &ind, QItemSelection &sel, QItemSelectionModel::SelectionFlags fl){
        if(model()->canFetchMore(ind)) model()->fetchMore(ind);
        sel.merge(QItemSelection(model()->index(0, 0, ind),
                                 model()->index(model()->rowCount(ind) - 1, 0, ind)), fl);
        for(int i = 0; i < model()->rowCount(ind); i++)
            _append_children(model()->index(i, 0, ind), sel, fl);
    }

};


}}}


#endif // GUI_FUNCTIONALITY

#endif // GTREEVIEW_H
