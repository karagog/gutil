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

#include <QObject>
#include <QAction>
#include <QList>
#include "gutil_macros.h"

class QActionGroup;

namespace GUtil{

namespace DataAccess{
    class GConsoleIODevice;
}


namespace BusinessObjects{


class CommandLineMenuItem
{
public:

    QString Text;
    QAction *Action;

    QList<CommandLineMenuItem *> SubMenus;

    CommandLineMenuItem(QObject *parent)
        :Action(new QAction(parent))
    {}
    CommandLineMenuItem(const CommandLineMenuItem &other)
        :Text(other.Text),
          Action(new QAction(other.Action->parent())),
          SubMenus(other.SubMenus)
    {}
    ~CommandLineMenuItem(){
        delete Action;
        foreach(CommandLineMenuItem *i, SubMenus)
            delete i;
    }

};




// This class will manage the navigation of command line menus.  To use it,
//  populate a menu structure hierarchy of CommandLineMenuItems, and subclass
//  the CommandLineMenu to override the menu_activated function to do something
//  interesting when an item is activated.
class CommandLineMenu :
        public QObject
{
    Q_OBJECT
public:

    explicit CommandLineMenu(CommandLineMenuItem *root_menu,
                             QObject *parent = 0);
    virtual ~CommandLineMenu(){
        delete _root;
    }

    // Starts the menu execution
    void Exec();

    inline CommandLineMenuItem *MenuRoot(){
        return _root;
    }


protected slots:

    virtual void menu_activated(QAction *);


private:

    CommandLineMenuItem *_root;
    CommandLineMenuItem *_current_item;

    static void _print_menu_item(DataAccess::GConsoleIODevice &,
                                 const CommandLineMenuItem &);

    static void _add_actions_to_group(const CommandLineMenuItem &,
                                      QActionGroup &);

};


}}

#endif // COMMANDLINEMENU_H
