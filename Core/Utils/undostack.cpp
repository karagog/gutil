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

NAMESPACE_GUTIL1(Utils);


void UndoStack::Push(IUndoableAction *cmd)
{
    cmd->Do();

    m_ptr++;

    if(m_ptr < (int)m_stack.Length())
    {
        // Erase the subsequent commands
        for(int i = m_ptr; i < (int)m_stack.Length(); ++i)
            delete m_stack[i];

        m_stack.Resize(m_ptr + 1);
        m_stack[m_ptr] = cmd;
    }
    else
    {
        GASSERT(m_ptr == (int)m_stack.Length());

        // Push the item on the end of the list
        m_stack.PushBack(cmd);
    }
}

void UndoStack::Clear()
{
    G_FOREACH(IUndoableAction *cmd, m_stack)
        delete cmd;

    m_stack.Clear();
}

void UndoStack::Undo()
{
    if(m_ptr >= 0)
    {
        m_stack[m_ptr]->Undo();
        m_ptr--;
    }
}

void UndoStack::Redo()
{
    int new_ptr( m_ptr + 1 );
    if(0 <= new_ptr && new_ptr < (int)m_stack.Length())
    {
        m_stack[new_ptr]->Do();
        m_ptr = new_ptr;
    }
}


END_NAMESPACE_GUTIL1;
