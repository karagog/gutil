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

#include <gutil/commandlinemenu.h>
#include <gutil/console.h>
USING_NAMESPACE_GUTIL;

CommandLineMenu::CommandLineMenu(CommandLineMenuItem *root)
    :_root(root),
      _current_item(_root)
{}

void CommandLineMenu::Exec()
{
    _current_item = _root;

    Forever([=]{
        GUINT32 num_children( _current_item->SubMenus.Length() );
        if(num_children == 0)
            return false;

        _print_menu_item(*_current_item);

        Forever([=]{
            Console::Write("Please Make a Selection: ");

            String input;
            do
            {
                input = Console::ReadLine();
            }
            while(input.IsEmpty());

            bool ok(false);
            GUINT32 index( input.ToInt(&ok) );
            if(!ok || index <= 0 || index > num_children)
            {
                Console::WriteLine(String::Format(
                                       "Invalid Input: '%s'.  "
                                       "Please enter a number between 1 and %d",
                                       input.ConstData(), num_children));
                return true;
            }

            index = index - 1;

            _current_item = _current_item->SubMenus[index];
            _current_item->Activated();
            return false;
        });
    });
}

#define MENU_WIDTH 45
void CommandLineMenu::_print_menu_item(const CommandLineMenuItem &mi)
{
    String tmps1('*', MENU_WIDTH);
    String tmps2(MENU_WIDTH);
    tmps2 = "*";
    tmps2.Append(String(' ', MENU_WIDTH - 2));
    tmps2.Append('*');

    Console::WriteLine("\n\n\n\n\n\n\n\n\n");
    Console::WriteLine(tmps1);

    Console::WriteLine(tmps2);
    Console::WriteLine(String::Format("* %s%s%s",
                                      mi.Text.ConstData(),
                                      String(' ', MENU_WIDTH - mi.Text.Length() - 3).ConstData(),
                                      "*"));
    Console::WriteLine(tmps2);
    Console::WriteLine(tmps1);

    for(GUINT32 i(0); i < mi.SubMenus.Length(); ++i)
        Console::WriteLine(String::Format("%d. %s", i + 1, mi.SubMenus[i]->Text.ConstData()));

    Console::WriteLine();
}
