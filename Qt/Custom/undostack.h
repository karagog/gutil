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

#ifndef GUTIL_UNDOSTACK_H
#define GUTIL_UNDOSTACK_H

#ifndef GUTIL_NO_GUI_FUNCTIONALITY

#include <QUndoStack>

namespace GUtil{ namespace QT{ namespace Custom{


/** Derive from this class to override the virtual methods.  The normal
    QUndoStack doesn't make these virtual
*/
class UndoStack :
        public QUndoStack
{
    Q_OBJECT
public:

    inline explicit UndoStack(QObject *parent = 0) :QUndoStack(parent){}

    /** Overridden from QUndoStack */
    virtual void push(QUndoCommand *cmd){ QUndoStack::push(cmd); }


public slots:

    /** Overridden from QUndoStack */
    virtual void redo(){ QUndoStack::redo(); }

    /** Overridden from QUndoStack */
    virtual void undo(){ QUndoStack::undo(); }

};


}}}


#endif // GUI_FUNCTIONALITY

#endif // GUTIL_UNDOSTACK_H
