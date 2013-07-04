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

#include "gutil_smartpointer.h"
#include <QTreeView>
#include <QMenu>

namespace GUtil{ namespace QT{ namespace Custom{


/** This is just a customized QTreeView, with extra functions
    I find to be useful.
*/
class TreeView :
        public QTreeView
{
    Q_OBJECT

    ::GUtil::Utils::SmartPointer<QMenu> m_contextMenu;

public:

    inline explicit TreeView(QWidget *parent = 0)
        :QTreeView(parent){}


    /** Sets the context menu to be shown when the user right-clicks the treeview.
        The view takes ownership of the menu, and deletes it when finished.
    */
    inline void SetContextMenu(QMenu *m){ m_contextMenu = m; }

    /** Returns the context menu which has been set, or 0 if none has been set. */
    inline QMenu *GetContextMenu(){ return m_contextMenu; }


public slots:

    /** Expands all parents in the tree until this index is visible */
    void ExpandToIndex(const QModelIndex &ind);

    /** Collapses the index and all its parents recursively */
    void CollapseWithParents(const QModelIndex &ind);

    /** Selects all the children recursively using the provided selection flags */
    void SelectChildrenOfIndex(QModelIndex ind,
                               int selection_flags = QItemSelectionModel::Select |
                                                     QItemSelectionModel::Rows);


protected:

    /** Overridden to show the context menu, if one has been set. */
    virtual void contextMenuEvent(QContextMenuEvent *);


private:

    void _append_children(const QModelIndex &, QItemSelection &, int);

};


}}}


#endif // GUI_FUNCTIONALITY

#endif // GUTIL_TREEVIEW_H
