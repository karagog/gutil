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

#include "undostack.h"
using namespace std;

NAMESPACE_GUTIL;


/** Helper function truncates the stack at the given index. */
static void __truncate_stack(Vector<IUndoableAction *> &s, int ind)
{
    if(0 > ind || ind >= (int)s.Length())
        return;

    for(int i = ind; i < (int)s.Length(); ++i)
        delete s[i];
    s.Resize(ind);
}


/** A private command class for executing macros. */
class __undoable_macro_command : public IUndoableAction
{
public:

    /** The constituent commands that make up this macro. */
    Vector<IUndoableAction *> Commands;

    virtual void Do()
    {
        // Nobody should ever call Do() on the macro command.  That would be a bug
        // They can only Undo() or Redo()
        GASSERT(false);
        throw NotImplementedException<>();
    }

    virtual void Undo()
    {
        ForEachReverse(Commands, [](IUndoableAction *a){
            a->Undo();
        });
    }

    virtual void Redo()
    {
        for(auto a : Commands)
            a->Redo();
    }

    virtual String Text() const
    {
        /** \todo Return a list of the commands contained within this macro. */
        return "";
    }

    virtual ~__undoable_macro_command()
    {
        for(auto a : Commands)
            delete a;
    }

};



UndoStack::UndoStack(function<void()> cb)
    :m_ptr(-1), m_macro(NULL), m_cb(cb)
{}

UndoStack::~UndoStack()
{
     Clear();
}

String UndoStack::GetUndoText() const
{
    return CanUndo() ? m_stack[m_ptr]->Text() : "";
}

String UndoStack::GetRedoText() const
{
    return CanRedo() ? m_stack[m_ptr + 1]->Text() : "";
}

void UndoStack::Do(IUndoableAction *cmd)
{
    Vector<IUndoableAction *> *vec;
    std::unique_ptr<IUndoableAction> ex_guard(cmd);
    cmd->Do();

    if(IsMakingMacro())
        vec = &reinterpret_cast<__undoable_macro_command *>(m_macro)->Commands;
    else
    {
        vec = &m_stack;
        m_ptr++;

        // Erase the subsequent commands
        __truncate_stack(m_stack, m_ptr);
    }

    // Push the item on the end of the list
    vec->PushBack(cmd);
    ex_guard.release();

    m_cb();
}

void UndoStack::Clear()
{
    for(auto cmd : m_stack)
        delete cmd;

    m_stack.Clear();
    m_ptr = -1;

    if(IsMakingMacro())
        delete reinterpret_cast<__undoable_macro_command *>(m_macro);

    m_cb();
}

void UndoStack::Undo()
{
    GASSERT(!IsMakingMacro());

    if(!IsMakingMacro())
    {
        if(0 <= m_ptr)
        {
            GASSERT(m_ptr < (int)m_stack.Length());

            m_stack[m_ptr]->Undo();
            m_ptr--;
        }
    }

    m_cb();
}

void UndoStack::Redo()
{
    GASSERT(!IsMakingMacro());

    if(!IsMakingMacro())
    {
        int new_ptr( m_ptr + 1 );
        if(new_ptr < (int)m_stack.Length())
        {
            GASSERT(0 <= new_ptr);

            m_stack[new_ptr]->Redo();
            m_ptr = new_ptr;
        }
    }

    m_cb();
}


void UndoStack::BeginMacro()
{
    if(IsMakingMacro()){
        GDEBUG("Cannot make nested macros...doing nothing instead");
    }
    else
        m_macro = new __undoable_macro_command;
}

void UndoStack::_end_macro(bool commit)
{
    // If this is called outside of macro generation, that must be a bug
    GASSERT(IsMakingMacro());

    if(!IsMakingMacro())
        return;

    __undoable_macro_command *c( reinterpret_cast<__undoable_macro_command *>(m_macro) );
    m_macro = NULL;

    if(0 < c->Commands.Length())
    {
        if(commit)
        {
            IUndoableAction *tmp = c;

            // If there is only one command in the macro, then only remember the one command
            if(1 == c->Commands.Length()){
                tmp = c->Commands[0];
                c->Commands.Clear();
                delete c;
            }

            m_ptr += 1;

            __truncate_stack(m_stack, m_ptr);

            m_stack.PushBack(tmp);
        }
        else
        {
            // If we are rolling back the macro, then undo all the previously-
            //  executed commands, starting at the end.
            ForEachReverse(c->Commands, [](IUndoableAction *a){
                a->Undo();
            });
            delete c;
        }
    }
    else
    {
        delete c;
    }

    m_cb();
}

bool UndoStack::IsMakingMacro()
{
    return m_macro;
}


END_NAMESPACE_GUTIL;
