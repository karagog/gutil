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

#ifndef ACTIONLOGBOOK_H
#define ACTIONLOGBOOK_H

#include <stack>

namespace GUtil
{
    namespace Core
    {
        namespace BusinessObjects
        {
            // Derive from this class to make action objects that undo what you want
            class action_t
            {
                friend class ActionStack;
            protected:
                // This function is used to carry out an action, or undo an action, depending
                //  on the boolean you pass to it
                virtual bool carry_out(bool) = 0;
            };


            // Use this class to manipulate a stack of actions
            class ActionStack
            {
            public:
                ActionStack();

                void pushAction(action_t *);
                action_t *popAction();

                bool undoLastAction();
                bool redoLastUndo();

                static bool carryOutAction(action_t *, bool undo = false);

            private:
                std::stack<action_t *> action_stack;
                std::stack<action_t *> redo_stack;

                static void clean_stack(std::stack<action_t *> &);
            };
        }
    }
}

#endif // ACTIONLOGBOOK_H
