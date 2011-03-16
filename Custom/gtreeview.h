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

#ifndef GTREEVIEW_H
#define GTREEVIEW_H

#include <QTreeView>

namespace GUtil{ namespace Custom{


class GTreeView :
        public QTreeView
{
    Q_OBJECT
public:

    explicit GTreeView(QWidget *parent = 0);

    // Selects all the children recursively using the provided selection flags
    void SelectChildrenOfIndex(QModelIndex, QItemSelectionModel::SelectionFlags);


public slots:

    // Expands all parents in the tree until this index is visible
    void ExpandToIndex(QModelIndex);

    // Collapses the index and all its parents recursively
    void CollapseWithParents(QModelIndex);

    // Clears the current selection and selects the row (current index will be
    //  on the row's first cell
    void SelectRow(QModelIndex);

    void SelectChildrenOfIndex(QModelIndex);


private:

    void _append_children(const QModelIndex &, QItemSelection &, QItemSelectionModel::SelectionFlags);

};


}}
#endif // GTREEVIEW_H
