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

#include "commandlinemenu.h"
#include "DataAccess/gconsoleiodevice.h"
#include <QActionGroup>
GUTIL_USING_NAMESPACE(BusinessObjects);
GUTIL_USING_NAMESPACE(DataAccess);

CommandLineMenu::CommandLineMenu(CommandLineMenuItem *root, QObject *parent)
    :QObject(parent),
      _root(root),
      _current_item(_root)
{
    // Connect all the actions to our menu_activated function
    QActionGroup *grp( new QActionGroup(this) );
    _add_actions_to_group(*_root, *grp);
    connect(grp, SIGNAL(triggered(QAction*)),
            this, SLOT(menu_activated(QAction*)));
}

void CommandLineMenu::_add_actions_to_group(const CommandLineMenuItem &item,
                                            QActionGroup &grp)
{
    grp.addAction(item.Action);

    for(int i = 0; i < item.SubMenus.length(); i++)
    {
        _add_actions_to_group(*item.SubMenus[i], grp);
    }
}

void CommandLineMenu::menu_activated(QAction *)
{
    // Base class does nothing.  Override to do something interesting
}

void CommandLineMenu::Exec()
{
    GConsoleIODevice cio;
    _current_item = _root;

    forever
    {
        int num_children( _current_item->SubMenus.length() );
        if(num_children == 0)
            break;

        _print_menu_item(cio, *_current_item);

        while(true)
        {
            cio.Write("Please Make a Selection: ");

            QString input(cio.ReadLine());
            if(!input.isEmpty())
            {
                bool ok(false);
                int index( input.toInt(&ok) );
                if(!ok || index <= 0 || index > num_children)
                {
                    cio.WriteLine(QString("Invalid Input: '%1'.  Please enter a number between 1 and %2")
                                  .arg(input)
                                  .arg(num_children));
                    continue;
                }

                _current_item = _current_item->SubMenus[index];

                // Activate the submenu that they just selected
                _current_item->Action->activate(QAction::Trigger);

                break;
            }
        }
    }
}

void CommandLineMenu::_print_menu_item(DataAccess::GConsoleIODevice &cio,
                                           const CommandLineMenuItem &mi)
{
    cio.WriteLine("**************************************************");
    cio.WriteLine("*");
    cio.Write("* ");  cio.Write(mi.Text.toAscii());
    cio.WriteLine("*");
    cio.WriteLine("**************************************************");
    cio.WriteLine("");

    for(int i = 0; i < mi.SubMenus.length(); i++)
    {
        cio.WriteLine(QString("%1. %2")
                      .arg(i + 1)
                      .arg(mi.SubMenus[i]->Text));
    }

    cio.WriteLine("");
}
