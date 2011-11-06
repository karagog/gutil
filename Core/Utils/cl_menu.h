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

#ifndef _GMENU_H
#define	_GMENU_H

#include <iostream>
#include <string>
#include <vector>

int get_input(std::istream & in, int & result, int lowlimit, int highlimit);
	//Get input between lowlimit and highlimit (inclusive)
		//Returns 0 on success
int show_menu(std::string menu_label,const std::vector<std::string> & items,int menu_width);
	//Given a vector of strings to use as menu items, this function displays
		//those menus items cleanly and gets user input.  The return value is
		//the index of the 'items' vector that the user selected.
	//Give it a menu_label to display as the title
bool get_yes_no(std::istream & str);
	//Gets a line from the stream and determines if the user typed 'yes'
		//or 'y' to select yes for something.  Returns true if YES


#endif	/* _GMENU_H */

