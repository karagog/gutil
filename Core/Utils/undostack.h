/*Copyright 2012 George Karagoulis
  
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

#include "gutil_vector.h"

NAMESPACE_GUTIL1(Utils);


/** An interface you must implement to give commands to the UndoStack. */
class IUndoableAction
{
public:

    /** Carries out the command. */
    virtual void Do() = 0;

    /** Undoes the command. */
    virtual void Undo() = 0;

    /** So the implementation class can be deleted by the interface. */
    virtual ~IUndoableAction(){}

};




/** Manages a stack of commands that can be undone and redone.

    You implement "command classes" that derive from IUndoableAction,
    and override the virtual functions.

    This class will manage the command class memory after you push it on the stack.

    The stack is guaranteed to be exception-safe, so you can throw exceptions
    during your command and they will safely propagate out without messing up
    the UndoStack.  When this happens, you can expect the logical behavior,
    which is that when you "push" a new command, if an exception is thrown it
    won't be added to the stack.  Also when you undo a command and an exception
    is thrown, it will be treated as if the command was never undone.

    All of the member functions are virtual, to allow you to build on the functionality
    and add interesting behavior to derived classes.

    \note The member functions are verbose (besides Undo() and Redo()) intentionally,
    because the class is designed to be inherited from, and this avoids naming collisions.
*/
class UndoStack
{
    ::GUtil::DataObjects::Vector<IUndoableAction *> m_stack;
    int m_ptr;

    GUTIL_DISABLE_COPY(UndoStack);
public:

    inline UndoStack() :m_ptr(-1){}
    virtual ~UndoStack(){ UndoStack::ClearUndoStack(); }

    /** Executes a command and pushes it onto the stack.

        Afterwards, its memory will belong to the UndoStack, assuming no exception is thrown.
    */
    virtual void PushUndoableAction(IUndoableAction *);

    /** Removes all commands from the stack without executing them. */
    virtual void ClearUndoStack();

    /** Undoes the item at the top of the stack, and decrements the stack pointer.
        \note Does nothing if the stack is empty
    */
    virtual void Undo();

    /** Redoes the item that was just popped off the stack, and adds it back on top of the stack.
        \note Does nothing if there are no more commands to be redone.
    */
    virtual void Redo();

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_UNDOSTACK_H
