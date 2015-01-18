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

#ifndef GUTIL_UNDOSTACK_H
#define GUTIL_UNDOSTACK_H

#include <gutil/vector.h>
#include <gutil/string.h>
#include <functional>

NAMESPACE_GUTIL;


/** An interface you must implement to give commands to the UndoStack. */
class IUndoableAction
{
public:

    /** Carries out the command for the first time.  After the first time, it is
        never executed again by the undo stack.
    */
    virtual void Do() = 0;

    /** Undoes the command, after having done it with Do() or Redo(). */
    virtual void Undo() = 0;

    /** Redoes the Undo() function.  This is called only after Do() was called
        the first time, and you undid it and now you want to do it again.
        The default implementation executes Do() again.
    */
    virtual void Redo(){ Do(); }

    /** A field to provide a textual description of the action. */
    virtual GUtil::String Text() const{ return String(); }

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

    You can make macro commands

    \note The member functions are verbose (besides Undo() and Redo()) intentionally,
    because the class is designed to be inherited from, and this avoids naming collisions.
*/
class UndoStack
{
    GUTIL_DISABLE_COPY(UndoStack);

    GUtil::Vector<IUndoableAction *> m_stack;
    int m_ptr;
    void *m_macro;
    std::function<void()> m_cb;

public:

    /** Constructs an undo stack with the given callback for when the
     *  stack changes
    */
    explicit UndoStack(std::function<void()> on_current_changed = []{});
    virtual ~UndoStack();


    /** Returns the current index of the undo stack. */
    int CurrentIndex() const{ return m_ptr; }

    /** Returns the current size of the undo stack (i.e. The total number of
        commands which have been pushed on the stack.)
    */
    int Size() const{ return (int)m_stack.Length(); }


    /** Returns true if you can undo a command on the stack. */
    bool CanUndo() const{ return 0 <= CurrentIndex(); }

    /** Returns true if you can redo a command on the stack. */
    bool CanRedo() const{ return CurrentIndex() < Size() - 1; }


    /** Returns a textual description of the current action to be undone.
     *  \note The undoable action must have implemented a text description.
    */
    virtual GUtil::String GetUndoText() const;

    /** Returns a textual description of the current action to be redone.
     *  \note The undoable action must have implemented a text description.
    */
    virtual GUtil::String GetRedoText() const;


    /** Executes a command and pushes it onto the stack.

        Afterwards, its memory will belong to the UndoStack, assuming no exception is thrown.
    */
    virtual void Do(IUndoableAction *);

    /** Removes all commands from the stack without executing them.
        This also deletes a macro if you were building one.
    */
    virtual void Clear();

    /** Undoes the item at the top of the stack, and decrements the stack pointer.
        \note Does nothing if the stack is empty
        \note Does nothing if a macro is being generated
    */
    virtual void Undo();

    /** Redoes the item that was just popped off the stack, and adds it back on top of the stack.
        \note Does nothing if there are no more commands to be redone.
        \note Does nothing if a macro is being generated
    */
    virtual void Redo();



    /** Begins a macro command, which consists of multiple commands executed
        sequentially.  When you undo a macro command, the constituend commands
        are undone in LIFO order (like a stack, rather than a queue).

       You may not nest calls to BeginMacro().  You can call it once, and then
       commit it or roll it back.

       You cannot Undo() or Redo() while composing a macro.

        \sa CommitMacro(), RollbackMacro()
    */
    void BeginMacro();

    /** Ends a macro command, and commits it to the undo stack.

        \sa BeginMacro(), RollbackMacro()
    */
    void CommitMacro(){ _end_macro(true); }

    /** Ends a macro command, rolling it back and not pushing it on the stack.

        The effect should be that nothing changed since the start of the macro.

        \sa BeginMacro(), CommitMacro()
    */
    void RollbackMacro(){ _end_macro(false); }

    /** Returns true if we're in the middle of generating a macro.

        If you are making nested macros, then the top-most macro must be ended
        before this function returns false.
    */
    bool IsMakingMacro();


private:

    void _end_macro(bool commit);

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_UNDOSTACK_H
