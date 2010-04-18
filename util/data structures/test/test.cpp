#include <iostream>
#include <string>
#include "../b_tree.h"
//#include "../catalogue_t.h"

using namespace std;

bool compare(string * a,string * b);
void showfunc(string * tmp);
int new_search(node_t<string *> * targ,string * obj,vector<string *> & results,bool (*cmp)(string*,string*));

int main(){


	string * temp = new string;
	bst_t<string *> mytree(&compare,&new_search);
//	catalogue_t mytree;
	vector<string *> results;

	while(1){
		cout<<"Enter something to push onto the tree: ";
		cin>>*temp;
		if(*temp == "quit")
			break;
		else if(*temp == "show"){
			mytree.show_df(&showfunc);
			mytree.show_bf(&showfunc);
			cout<<endl;
		}
		else if(*temp == "search"){
			do{
				cout<<"Search for what?: ";
				cin>>*temp;
				if(mytree.search(temp,results) != 0){
					cout<<"Found results: "<<endl;
					for(unsigned int i = 0; i < results.size(); i++)
						cout<<i<<". "<<*(results[i])<<endl;
				}
				else{
					cout<<"No Results..."<<endl;
				}
				results.resize(0);
			}while(*temp != "quit");
		}
		else if(*temp == "delete"){
			do{
				cout<<"Delete what?: ";
				cin>>*temp;
				if(mytree.remove(temp,0) == ELT_NOT_FOUND){
					cout<<"Element not found in the tree..."<<endl;
				}
			}while(*temp != "quit");

		}
		else{
			if(mytree.add(temp) == NODE_DUPLICATE)
				cout<<"Uh Oh..."<<endl;
			temp = new string;
		}
	}
	return 0;
}

bool compare(string * a,string * b){
	//cout<<"Comparing "<<*a<<" with "<<*b<<endl;
	return (*a < *b);
}

void showfunc(string * tmp){
	cout<<*tmp;
}

int new_search(node_t<string *> * targ,string * obj,vector<string *> & results,bool (*cmp)(string*,string*)){
	string * temp;
	if(targ->key(temp) == NODE_EMPTY){
		return -1;
	}

	if(cmp(obj,temp)){
		return(new_search(targ->get_lchild(),obj,results,cmp));
	}
	else if(cmp(temp,obj)){
		return(new_search(targ->get_rchild(),obj,results,cmp));
	}
	else{			//They're equal
		vector<string *> * tmpcontents = targ->get_contents();
		unsigned int sz = targ->get_contents()->size();
		for (unsigned int i = 0; i < sz; i++){
			results.push_back(tmpcontents->at(i));
		}
		return 0;
	}
}

