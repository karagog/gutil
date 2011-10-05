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

#ifndef COMMANDLINEMENU_H
#define COMMANDLINEMENU_H

#include <QString>
#include <QList>
#include "gutil.h"

class QActionGroup;

namespace GUtil{

namespace DataAccess{
    class GConsoleIODevice;
}


namespace Utils{


// This class will manage the navigation of command line menus.  To use it,
//  populate a menu structure hierarchy of CommandLineMenuItems, and subclass
//  the CommandLineMenu to override the menu_activated function to do something
//  interesting when an item is activated.
class CommandLineMenu
{
public:

    class CommandLineMenuItem
    {
    public:

        QString Text;
        QList<CommandLineMenuItem *> SubMenus;

        virtual ~CommandLineMenuItem(){
            foreach(CommandLineMenuItem *i, SubMenus)
                delete i;
        }

    };


    explicit CommandLineMenu(CommandLineMenuItem *root_menu);
    virtual ~CommandLineMenu(){
        delete _root;
    }

    // Starts the menu execution
    void Exec();

    // Resets back to the main menu
    void Reset();

    inline CommandLineMenuItem *MenuRoot(){
        return _root;
    }


protected:

    virtual void menu_activated(CommandLineMenuItem *);


private:

    CommandLineMenuItem *_root;
    CommandLineMenuItem *_current_item;

    static void _print_menu_item(const CommandLineMenuItem &);

};


}}

#endif // COMMANDLINEMENU_H
