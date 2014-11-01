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

#ifndef GUTIL_TREEVIEW_H
#define GUTIL_TREEVIEW_H

#ifndef GUTIL_NO_GUI_FUNCTIONALITY

#include <QTreeView>

namespace GUtil{ namespace Qt{


/** This is just a customized QTreeView, with extra functions
    I find to be useful.
*/
class TreeView :
        public QTreeView
{
    Q_OBJECT

public:

    explicit TreeView(QWidget *parent = 0)
        :QTreeView(parent){}

public slots:

    /** Expands all parents in the tree until this index is visible */
    void ExpandToIndex(const QModelIndex &ind);

    /** Collapses the index and all its parents recursively */
    void CollapseWithParents(const QModelIndex &ind);

    /** Selects all the children recursively using the provided selection flags */
    void SelectChildrenOfIndex(QModelIndex ind,
                               int selection_flags = QItemSelectionModel::Select |
                                                     QItemSelectionModel::Rows);


private:

    void _append_children_to_selection(const QModelIndex &, QItemSelection &, int);

};


}}


#endif // GUI_FUNCTIONALITY

#endif // GUTIL_TREEVIEW_H
