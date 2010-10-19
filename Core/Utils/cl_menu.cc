#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include "stringhelpers.h"
#include "cl_menu.h"
using namespace std;
using namespace GUtil::Core;

int get_input(istream & in, int & result, int lowlimit, int highlimit){
	//This attempts to get an integer input from the provided stream.
	//If it got a valid integer then it converts it from a string for you and
		//puts it in 'result' and returns 0.  It continually gets lines from the
		//'in' stream until a valid integer is received.
	//You provide the upper and lower bounds of this integer.  This function will
		//continue polling the input stream until an integer is received that is in
		//these bounds.  (lowlimit <= result <= highlimit)
	//If for some reason the stream errors out (end of file, etc...) this function
		//returns -1.  Other than that there is no way it can return anything but 0

	string templine, tempselection;
	int selection;

	cout<<endl<<"Please make a selection: ";
	getline(in,templine);
	while(!in.fail()){
		istringstream tempstream(templine.c_str());				//Turn the line into a stream
		tempstream >> tempselection;						//Grab the first word from the line (until whitespace)
                if(Tools::StringHelpers::toInt(tempselection,selection)){		//If the selection was a valid number
			if((selection >= lowlimit) && (selection <= highlimit)){	//If the selection is in bounds
				//result = selection - 1;
				result = selection;
				return 0;
			}
		}
		cout<<"INVALID SELECTION"<<endl<<endl;
		cout<<"Please make a different selection: ";
		getline(in,templine);
	}
	return(-1);
}

int show_menu(string menu_label,const vector<string> & items,int menu_width){
	//This function displays the menu items supplied, gets input from the user
		//and returns the index of the item that the user selected
	int selection = 0;

	menu_label = "*         " + menu_label;

	cout<<setfill('*')<<setw(menu_width)<<right<<"*"<<endl;
	cout<<setfill(' ')<<setw(menu_label.size())<<menu_label.c_str()<<setw(menu_width - menu_label.size())<<"*"<<endl;
	cout<<setfill('*')<<setw(menu_width)<<"*"<<endl<<endl;

	for(unsigned int i = 0; i < items.size(); i++){
		cout<<"\t"<<setw(3)<<setfill(' ')<<left<<i+1<<" "<<items[i]<<endl;
	}

	if(get_input(cin,selection,1,items.size()) != 0){
		cout<<"ERROR: There was an unexpected error getting the selection...aborting..."<<endl;
		exit(1);
	}
	return selection;
}

bool get_yes_no(istream & str){
	//Grabs a line from the provided stream and determines if the user said yes
	//It determines this by checking the first word
	string templine;
	string tempword = "";
	string first_word;

	getline(str,templine);
	if(str.fail()){
		cout<<"ERROR: (get_yes_no)Could not get answer from provided stream"<<endl;
		return(false);
	}

	istringstream tempstream(templine.c_str());
	tempstream>>tempword;
	first_word = tempword;
	while(!tempstream.fail()){
		if(tempword != first_word){			//This means the user input too many words, so it can't be a valid 'yes' or 'no'
			return(false);
		}
		tempstream>>tempword;
	}

	//At this point first_word is the word of interest; parse this for a valid 'yes'
        first_word = Tools::StringHelpers::toLower(first_word);
	if(first_word == "yes" || first_word == "y")
		return(true);
	return(false);
}

