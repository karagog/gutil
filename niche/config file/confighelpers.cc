#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "Gcfg.h"
using namespace std;

int parse_cfg_line(const string & line,string & lhs,vector<string> & rhs){
	unsigned int center;
	string tmp;
	string rh;

	lhs = line.substr(0,line.find(CFG_COMMENT_MARK));
	rhs.resize(0);			//To spread out dependent instructions
	center = lhs.find("=");
	rh = lhs.substr(center + 1);
	lhs = lhs.substr(0,center);

	istringstream lhss(lhs.c_str());
	istringstream rhss(rh.c_str());
	lhss>>lhs;
	if(lhss.fail() || (lhs.size() == 0))
		return -1;

	rhss>>rh;
	while(!rhss.fail()){
		while(rh.size() > 0){
			center = rh.find(ELT_DELIMITER);	//Not really the 'center', I'm just reusing the variable
			tmp = rh.substr(0,center);
			rhs.push_back(tmp);
			if(center == string::npos)
				rh = "";
			else
				rh = rh.substr(center + 1);
		}
		rhss>>rh;
	}
	return 0;
}

