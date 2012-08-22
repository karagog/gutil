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

#include "undostack.h"
USING_NAMESPACE_GUTIL1(DataObjects);

NAMESPACE_GUTIL1(Utils);


/** A private command class for executing macros. */
class __undoable_macro_command : public IUndoableAction
{
public:

    /** The constituent commands that make up this macro. */
    Vector<IUndoableAction *> Commands;

    virtual void Do()
    {
        // Nobody should ever call do on the macro command.  That would be a bug
        // They can only Undo() or Redo()
        GDEBUG(false);
    }

    virtual void Undo()
    {
        G_FOREACH_REVERSE(IUndoableAction *a, Commands)
            a->Undo();
    }

    virtual void Redo()
    {
        G_FOREACH(IUndoableAction *a, Commands)
            a->Redo();
    }

    virtual ~__undoable_macro_command()
    {
        G_FOREACH(IUndoableAction *a, Commands)
            delete a;
    }

};




UndoStack::UndoStack()
    :m_ptr(-1)
{}

UndoStack::~UndoStack()
{
     Clear();
}

void UndoStack::Do(IUndoableAction *cmd)
{
    Vector<IUndoableAction *> *vec;

    cmd->Do();

    if(IsMakingMacro())
        vec = &reinterpret_cast<__undoable_macro_command *>(m_macros.Back())->Commands;
    else
    {
        vec = &m_stack;
        m_ptr++;

        if(m_ptr < (int)m_stack.Length())
        {
            // Erase the subsequent commands
            for(int i = m_ptr; i < (int)m_stack.Length(); ++i)
                delete m_stack[i];

            m_stack.Resize(m_ptr);
        }

        GASSERT(m_ptr == (int)m_stack.Length());
    }

    // Push the item on the end of the list
    vec->PushBack(cmd);
}

void UndoStack::Clear()
{
    G_FOREACH(IUndoableAction *cmd, m_stack)
        delete cmd;

    m_stack.Clear();
    m_ptr = -1;

    if(IsMakingMacro())
    {
        G_FOREACH(void *v, m_macros)
            delete reinterpret_cast<__undoable_macro_command *>(v);

        m_macros.Clear();
    }
}

void UndoStack::Undo()
{
    GASSERT(!IsMakingMacro());

    if(!IsMakingMacro())
    {
        if(m_ptr >= 0)
        {
            m_stack[m_ptr]->Undo();
            m_ptr--;
        }
    }
}

void UndoStack::Redo()
{
    GASSERT(!IsMakingMacro());

    if(!IsMakingMacro())
    {
        int new_ptr( m_ptr + 1 );
        if(0 <= new_ptr && new_ptr < (int)m_stack.Length())
        {
            m_stack[new_ptr]->Redo();
            m_ptr = new_ptr;
        }
    }
}


void UndoStack::BeginMacro()
{
    m_macros.PushBack(new __undoable_macro_command);
}

void UndoStack::_end_macro(bool commit)
{
    // If this is called outside of macro generation, that must be a bug
    GASSERT(IsMakingMacro());

    if(!IsMakingMacro())
        return;

    __undoable_macro_command *c( reinterpret_cast<__undoable_macro_command *>(m_macros.Back()) );
    m_macros.PopBack();

    if(0 < c->Commands.Count())
    {
        Vector<IUndoableAction *> *vec;

        if(IsMakingMacro())
            vec = &reinterpret_cast<__undoable_macro_command *>(m_macros.Back())->Commands;
        else
        {
            vec = &m_stack;
            if(commit)
                ++m_ptr;
        }

        if(commit)
            vec->PushBack(c);
        else
        {
            // If we are rolling back the macro, then undo all the previously-
            //  executed commands, starting at the end.
            G_FOREACH_REVERSE(IUndoableAction *a, *vec){
                a->Undo();
            }
            delete c;
        }
    }
    else
    {
        delete c;
    }
}

bool UndoStack::IsMakingMacro()
{
    return 0 < m_macros.Count();
}


END_NAMESPACE_GUTIL1;
