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

#ifndef GUNDOSTACK_H
#define GUNDOSTACK_H

#include <QUndoStack>

namespace GUtil{ namespace GUICustom{


// Derive from this class to override the virtual methods.  The normal
//  QUndoStack doesn't make these virtual
class GUndoStack :
        public QUndoStack
{
    Q_OBJECT
public:

    explicit GUndoStack(QObject *parent = 0);

    virtual void push(QUndoCommand *);


public slots:

    virtual void redo();
    virtual void undo();

};


}}

#endif // GUNDOSTACK_H