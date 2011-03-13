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
#include <iostream>
#include <iomanip>
using namespace std;
GUTIL_USING_NAMESPACE(Utils);

CommandLineMenu::CommandLineMenu(CommandLineMenuItem *root)
    :_root(root),
      _current_item(_root)
{}

void CommandLineMenu::menu_activated(CommandLineMenuItem *)
{
    // Base class does nothing.  Override to do something interesting
}

void CommandLineMenu::Exec()
{
    _current_item = _root;

    forever
    {
        int num_children( _current_item->SubMenus.length() );
        if(num_children == 0)
            break;

        _print_menu_item(*_current_item);

        do
        {
            cout<<"Please Make a Selection: ";

            QString input;
            do
            {
                string s;
                cin >> s;
                input = QString::fromStdString(s);
            }
            while(input.isEmpty());

            bool ok(false);
            int index( input.toInt(&ok) );
            if(!ok || index <= 0 || index > num_children)
            {
                cout<<QString("Invalid Input: '%1'.  Please enter a number between 1 and %2")
                       .arg(input)
                       .arg(num_children).toStdString()<<endl;
                continue;
            }

            index = index - 1;
            _current_item = _current_item->SubMenus[index];

            menu_activated(_current_item);

            break;
        }while(true);
    }
}

void CommandLineMenu::Reset()
{
    _current_item = _root;
}

#define MENU_WIDTH 45
void CommandLineMenu::_print_menu_item(const CommandLineMenuItem &mi)
{
    cout<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl;

    cout<<setw(MENU_WIDTH)<<setfill('*')<<""<<endl;
    cout<<"*"<<setfill(' ')<<setiosflags(ios_base::right)<<setw(MENU_WIDTH - 1)<<"*"<<endl;
    cout<<setiosflags(ios_base::left)<<
           QString("* %1")
           .arg(mi.Text).toStdString()
        <<setiosflags(ios_base::right)<<setw(MENU_WIDTH - mi.Text.length() - 2)<<"*"<<endl;
    cout<<"*"<<setiosflags(ios_base::right)<<setw(MENU_WIDTH - 1)<<"*"<<endl;
    cout<<setw(MENU_WIDTH)<<setfill('*')<<""<<endl<<endl;

    for(int i = 0; i < mi.SubMenus.length(); i++)
    {
        cout<<QString("%1. %2")
                      .arg(i + 1)
                      .arg(mi.SubMenus[i]->Text).toStdString()<<endl;
    }

    cout<<endl;
}
