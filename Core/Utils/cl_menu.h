/* 
 * File:   Gmenu.h
 * Author: george
 *
 * Created on November 29, 2009, 4:52 PM
 */

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

