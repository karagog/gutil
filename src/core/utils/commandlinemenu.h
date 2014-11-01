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

#ifndef GUTIL_COMMANDLINEMENU_H
#define GUTIL_COMMANDLINEMENU_H

#include <gutil/strings.h>
#include <gutil/vector.h>
#include <gutil/smartpointer.h>
#include <gutil/macros.h>

NAMESPACE_GUTIL;


/** This class will manage the navigation of command line menus.  To use it,
    populate a menu structure hierarchy of CommandLineMenuItems, and subclass
    the CommandLineMenu to override the menu_activated function to do something
    interesting when an item is activated.

    This class takes ownership of the menu object memory.
*/
class CommandLineMenu
{
public:

    /** Represents one level of the menu hierarchy. */
    class CommandLineMenuItem
    {
    public:

        /** The name of the menu (displayed at the top). */
        String Text;

        /** A list of choices that will navigate to other menus, or trigger events.s */
        Vector< CommandLineMenuItem *> SubMenus;

        /** This function is called when the menu gets activated.
            You can override it to do whatever you want.
        */
        virtual void Activated(){}

        virtual ~CommandLineMenuItem(){
            for(Vector<CommandLineMenuItem *>::iterator iter(SubMenus.begin());
                iter != SubMenus.end(); ++iter)
                delete *iter;
        }

    };


    /** You must construct the Menu object with a root menu. */
    explicit CommandLineMenu(CommandLineMenuItem *root_menu);

    /** Executes the menu; displays it on the console */
    void Exec();

    /** The top-level menu. */
    CommandLineMenuItem const *MenuRoot() const{
        return _root;
    }
    /** The top-level menu. */
    CommandLineMenuItem *MenuRoot(){
        return _root;
    }


private:

    SmartPointer<CommandLineMenuItem> _root;
    CommandLineMenuItem *_current_item;

    static void _print_menu_item(const CommandLineMenuItem &);

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_COMMANDLINEMENU_H
