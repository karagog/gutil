/*Copyright 2010 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "actionstack.h"
using namespace std;
using namespace GUtil::Core::Utils;

ActionStack::ActionStack()
{
}

void ActionStack::pushAction(action_t *a)
{
    if(a == 0)
        return;

    action_stack.push(a);

    if(redo_stack.size() > 0)
        clean_stack(redo_stack);
}

action_t *ActionStack::popAction()
{
    action_t *ret = 0;
    if(action_stack.size() > 0)
    {
        ret = action_stack.top();
        action_stack.pop();
    }

    // Clear the redo stack
    if(redo_stack.size() > 0)
        clean_stack(redo_stack);

    return ret;
}

bool ActionStack::carryOutAction(action_t *a, bool undo)
{
    return a->carry_out(!undo);
}

bool ActionStack::undoLastAction()
{
    if(action_stack.size() == 0)
        return false;

    action_t *ret = action_stack.top();
    redo_stack.push(ret);
    return ret->carry_out(false);
}

bool ActionStack::redoLastUndo()
{
    if(redo_stack.size() == 0)
        return false;

    action_t *a = redo_stack.top();
    action_stack.push(a);
    return a->carry_out(true);
}

void ActionStack::clean_stack(std::stack<action_t *> &st)
{
    while(st.size() > 0)
    {
        action_t *a = st.top();
        st.pop();
        delete a;
    }
}
