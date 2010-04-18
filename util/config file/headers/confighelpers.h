/* 
 * File:   Gcfg.h
 * Author: george
 *
 * Created on November 29, 2009, 4:40 PM
 */

#ifndef _GCFG_H
#define	_GCFG_H

#include <string>
#include <vector>
using namespace std;

//Helper functions for easier config file parsing
	//Expects config file entries of the format:
		//<field> = <elt1> <elt2> <elt3>
	//Note: Whitespace will delimit the 'elt's, but so will commas.  There is no escape for
		//a whitespace at this point.

#define CFG_COMMENT_MARK "#"
#define ELT_DELIMITER ","	//What separates the elements in a field

int parse_cfg_line(const string & line,string & lhs,vector<string> & rhs);
	//parse_cfg_line takes a line of the form: <lhs> = <rhs1>, <rhs2>, ... <rhsN>
		//and returns the values of 'lhs' and 'rhs' in the appropriate variables.
		//If for some reason it can't parse the line (misunderstood format?  Blank line?)
		//then it returns less than 0.

		//This function is pretty robust, so a config line could have lots of separating
		//whitespace and this will still be able to read it.  As an example, even the
		//following line is a valid input line to this function:
			//EXAMPLE: field1 = arg1,arg2 arg3,arg4,arg5   arg6,arg7,arg8,	arg9


#endif	/* _GCFG_H */

