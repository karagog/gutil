#include <vector>
#include <string>
#include <cstdlib>
#include <cassert>
#include <iostream>
using namespace std;

#ifndef H_BST_T
#define H_BST_T

//Error codes that you can use in your code
#define ELT_NOT_FOUND		-2
#define ELT_ALREADY_EXISTS	-1

#define NODE_EMPTY -1
#define NODE_DUPLICATE -2

#define RIGHT_HEAVY	2
#define LEFT_HEAVY	1
#define BALANCED	0

#define uint unsigned int

template<class T>class node_t{
public:
	int add(const T & object,bool (*cmp)(const T,const T));
	int remove(const T & obj,int index,bool (*cmp)(const T,const T));
	node_t<T> operator = (const node_t<T> & rhs);
	void export_df(vector<T> & result);
	int key(T & result);
	int num_elts(){return contents.size();}
	void show_df(void (*showfunc)(T));
	void show_bf(void (*showfunc)(T));
	void balance();
	void rotate();
	int  balanced();								//Returns 0 if balanced, 1 if left-heavy and 2 if right-heavy
	node_t<T> * min_elt();			//Returns a pointer to the minimum element in the tree
	node_t<T> * max_elt();
	bool right_child(node_t<T> * compare){return(compare == r_child);}		//Returns true if 'compare' is this node's right child
	uint get_height(){return height;}
	uint get_rheight(){return r_child->height;}
	uint get_lheight(){return l_child->height;}
	void set_height(uint h){height = h;}
	node_t<T> * get_parent(){return parent;}
	node_t<T> * get_rchild(){return r_child;}
	node_t<T> * get_lchild(){return l_child;}
	void set_parent(node_t<T> * other){parent = other;}
	void set_rchild(node_t<T> * other){r_child = other;}
	void set_lchild(node_t<T> * other){l_child = other;}
	vector<T> * get_contents(){return(&contents);}
	void delete_children();
	node_t();
	node_t(const T & object);
	~node_t();
private:
	node_t<T> * parent;
	node_t<T> * l_child;
	node_t<T> * r_child;
	vector<T> contents;
	uint height;
};

template<class T>class bst_t{
public:
	int add(const T & object);						//Makes a copy of object and puts in the tree. If already exists in the tree, returns error
	int search(const T & obj,vector<T> & results);			//Make 'result' equal the resuls of the search; return null vector if not found
	int remove(const T & object,int index);
	void export_df(vector<T> & result);
	void show_df(void (*showfunc)(T));
	void show_bf(void (*showfunc)(T));
	node_t<T> * get_root_node(){return root;}
	bool (*cmp)(const T lhs,const T rhs);
	bst_t();
	bst_t(bool (*compare)(const T,const T));
	bst_t(bool (*compare)(const T,const T),int (*nsearch)(node_t<T> *,T,vector<T> &,bool (*)(const T, const T)));
	~bst_t();
private:
	node_t<T> * root;
	int (*search_alg)(node_t<T> * targ,T obj,vector<T> & results,bool (*cmp)(const T, const T));
};

template<class T>int node_t<T>::add(const T & obj,bool (*cmp)(const T,const T)){
	int retval = 0;
	T temp;
	if(key(temp) != NODE_EMPTY){
		if(cmp(obj,temp))
			return(l_child->add(obj,cmp));
		else if(cmp(temp,obj))		//obj > temp
			return(r_child->add(obj,cmp));
	}
	//If we made it this far, then we are ok to insert the object here, because either
		//we've evaluated them to be equal, or it's an empty node.
	contents.push_back(obj);
	if(height == 0){				//If this node was previously empty then now it has h=1, otherwise height stays the same
		height = 1;
		l_child = new node_t<T>();		//When we populate a new node, we also create its children automatically
		r_child = new node_t<T>();
		l_child->parent = this;
		r_child->parent = this;
	}
	else{
		retval = NODE_DUPLICATE;
	}
	//Need to balance before we're finished
	if(parent != 0)
		parent->balance();
	return retval;
}

template<class T>int node_t<T>::remove(const T & obj,int index,bool (*cmp)(const T,const T)){
	T myKey,childsKey;
	assert(key(myKey) != NODE_EMPTY);

	node_t<T> ** targetptr;
	targetptr = 0;
	if(cmp(obj,myKey)){
		targetptr = &(l_child);
	}
	else if(cmp(myKey,obj)){
		targetptr = &(r_child);
	}
	assert(targetptr != 0);			//This is because it should have evalutated true at one step higher than this
	node_t<T> * copy = *(targetptr);		//A copy of the node we're about to delete

	if((*targetptr)->key(childsKey) == NODE_EMPTY)
		return ELT_NOT_FOUND;

	if(cmp(childsKey,obj) || cmp(obj,childsKey)){		//Our child is not the target key, so descend down into it
		return((*targetptr)->remove(obj,index,cmp));
	}
	else{
		(*targetptr)->contents.erase((*targetptr)->contents.begin() + index);
		if((*targetptr)->contents.size() == 0){				//If we just erased the last element in the node
			int lheight = (*targetptr)->l_child->height, rheight = (*targetptr)->r_child->height;
			if(lheight == 0 && rheight == 0){			//if the node is a leaf, just delete it.
				delete (*targetptr)->l_child;
				delete (*targetptr)->r_child;
				(*targetptr)->l_child = (*targetptr)->r_child = 0;
				(*targetptr)->height = 0;
				balance();
			}
			else{
				node_t<T> * parA,* lchildA,* rchildA;
				node_t<T> * parB,* lchildB,* rchildB;
				node_t<T> * replacement;
				bool rightHeavy = false;

				parA	= (*targetptr)->parent;
				lchildA = (*targetptr)->l_child;
				rchildA = (*targetptr)->r_child;
				if(lheight <= rheight){			//The node is either balanced or right-heavy
					rightHeavy = true;
					replacement = (*targetptr)->r_child->min_elt();

					delete replacement->l_child;	//Since this is the min elt, we delete its blank neighbor
					replacement->l_child = 0;
				}
				else{						//It's left-heavy
					replacement = (*targetptr)->l_child->max_elt();

					delete replacement->r_child;	//Since this is the max elt, we delete its blank neighbor
					replacement->r_child = 0;
				}
				parB	= replacement->parent;
				lchildB = replacement->l_child;
				rchildB = replacement->r_child;
				//reference = parB;					//to remember where to balance from

				//Now patch in the replacement node:
				replacement->parent = parA;
				(*targetptr) = replacement;
				if((replacement == copy->l_child) || (replacement == copy->r_child)){		//The special case where the replacement is directly ajacent to this node
					if(rightHeavy){
						replacement->l_child = lchildA;
						lchildA->parent = replacement;
					}
					else{
						replacement->r_child = rchildA;
						rchildA->parent = replacement;
					}
					parB = replacement;				//because at this point parB is off the tree, and we need to balance from where it left
				}
				else{
					replacement->l_child = lchildA;
					replacement->r_child = rchildA;
					rchildA->parent = replacement;
					lchildA->parent = replacement;

					if(!rightHeavy){			//If replacement was its parent's right child
						parB->r_child = lchildB;
						lchildB->parent = parB;
					}
					else{						//else it was its parent's left child
						parB->l_child = rchildB;
						rchildB->parent = parB;
					}
				}
				//reference->balance();
				delete copy;
				parB->balance();
			}
		}
	}
	return 0;
}

template<class T>int node_t<T>::key(T & result){		//Returns one element in the array
	if(contents.size() > 0){
		result = contents[0];
		return 0;
	}
	return NODE_EMPTY;
}

template<class T>node_t<T> node_t<T>::operator = (const node_t<T> & rhs){
	parent = rhs.parent;
	l_child = rhs.l_child;
	r_child = rhs.r_child;
	contents = rhs.contents;
	return(*this);
}

template<class T>void node_t<T>::export_df(vector<T> & result){
	T temp;
	if(key(temp) == NODE_EMPTY)
		return;
	l_child->export_df(result);

	result.push_back(temp);
	r_child->export_df(result);
}

template<class T>void node_t<T>::show_df(void (*showfunc)(T)){
	if(contents.size() == 0)		//Empty node
		return;
	l_child->show_df(showfunc);
	for(uint i = 0; i < contents.size(); i++)
		showfunc(contents[i]);
	cout<<" "<<endl;
	//cout<<"\t\tHEIGHT: "<<height<<endl;
	r_child->show_df(showfunc);
}

template<class T>void node_t<T>::show_bf(void (*showfunc)(T)){
	T temp;
	if(key(temp) == NODE_EMPTY){
		return;
	}
	cout<<"(";
	showfunc(temp);
	cout<<",";
	l_child->show_bf(showfunc);
	cout<<",";
	r_child->show_bf(showfunc);
	cout<<")";
}

template<class T>void node_t<T>::balance(){			//This function also updates the "height" variables
	if(contents.size() == 0){
		height = 0;
		if(parent != 0)
			parent->balance();
		return;
	}

	int lheight = 0,rheight = 0;
	int diff = 0;

	lheight = l_child->get_height();		//These are guaranteed not to segfault because we exit above if this is an empty node
	rheight = r_child->get_height();

	diff = rheight - lheight;
	if(diff < -1){					//This node is left-heavy
		if(l_child->balanced() == RIGHT_HEAVY){
			l_child->rotate();
		}
		rotate();
	}
	else if(diff > 1){				//This node is right-heavy
		if(r_child->balanced() == LEFT_HEAVY){
			r_child->rotate();
		}
		rotate();
	}
	else{							//Else it's balanced somewhere in the middle
		height = lheight;
		if(rheight > lheight)
			height = rheight;
		height++;
		if(parent != 0)
			parent->balance();
		return;
	}
	balance();	//Need to rebalance if we just rotated a leg
}

template<class T>node_t<T> * node_t<T>::min_elt(){
	if(height == 0)
		return(parent);
	return(l_child->min_elt());
}

template<class T>node_t<T> * node_t<T>::max_elt(){
	if(height == 0)
		return(parent);
	return(r_child->max_elt());
}


template<class T>void node_t<T>::rotate(){
	int bal = balanced();

	if(bal == BALANCED){
		//cout<<"ERROR: You're trying to rotate a node that is perfectly balanced...what's the use?"<<endl;
		return;
	}

	if(bal == RIGHT_HEAVY){			//Right heavy...
		if(parent != 0){
			if(parent->right_child(this))			//If we are our parent's right child
				parent->set_rchild(r_child);
			else
				parent->set_lchild(r_child);
		}
		r_child->parent = parent;
		parent = r_child;
		r_child = parent->l_child;
		if(r_child != 0)
			r_child->parent = this;
		parent->l_child = this;
	}
	else{					//left heavy...
		if(parent != 0){
			if(parent->right_child(this))			//If we are our parent's right child
				parent->set_rchild(l_child);
			else
				parent->set_lchild(l_child);
		}
		l_child->parent = parent;
		parent = l_child;
		l_child = parent->r_child;
		if(l_child != 0)
			l_child->parent = this;
		parent->r_child = this;
	}
}

template<class T>int node_t<T>::balanced(){
	int lheight = 0, rheight = 0;
	if(l_child != 0)
		lheight = l_child->get_height();
	if(r_child != 0)
		rheight = r_child->get_height();

	if(rheight > lheight)
		return RIGHT_HEAVY;
	if(lheight > rheight)
		return LEFT_HEAVY;
	return BALANCED;
}

template<class T>void node_t<T>::delete_children(){
	if(l_child != 0){
		l_child->delete_children();
		delete l_child;
	}
	if(r_child != 0){
		r_child->delete_children();
		delete r_child;
	}
}

template<class T>node_t<T>::node_t(){
	parent = 0;
	l_child = 0;
	r_child = 0;
	height = 0;
}

template<class T>node_t<T>::node_t(const T & object){
	parent = 0;
	l_child = 0;
	r_child = 0;
	contents.push_back(object);
	height = 1;
}

template<class T>node_t<T>::~node_t(){}



template<class T>void bst_t<T>::show_df(void (*showfunc)(T)){
	root->show_df(showfunc);
}

template<class T>void bst_t<T>::show_bf(void (*showfunc)(T)){
	root->show_bf(showfunc);
}

template<class T>int bst_t<T>::add(const T & object){
	int retval = root->add(object,cmp);
	if(root->get_parent() != 0){
		root = root->get_parent();
		assert(root->get_parent() == 0);			//If we've just added 1 item, then we should not have to shift root more than 1 level
	}
	return(retval);
}

template<class T>int bst_t<T>::search(const T & obj,vector<T> & results){
	return(search_alg(root,obj,results,cmp));
}

template<class T>int bst_t<T>::remove(const T & obj,int index){
	T childsKey;

	node_t<T> ** targetptr = &root;
	node_t<T> * copy = *(targetptr);
	if((*targetptr)->key(childsKey) == NODE_EMPTY)
		return ELT_NOT_FOUND;

	if(cmp(obj,childsKey) || cmp(childsKey,obj)){		//Our child is not the target key, so descend down into it
		return((*targetptr)->remove(obj,index,cmp));
	}
	else{
		vector<T> * tempcontents = (*targetptr)->get_contents();
		tempcontents->erase(tempcontents->begin() + index);
		if(tempcontents->size() == 0){				//If we just erased the last element in the node
			int lheight = (*targetptr)->get_lheight(), rheight = (*targetptr)->get_rheight();
			if(lheight == 0 && rheight == 0){			//if the node is a leaf, just delete it.
				delete (*targetptr)->get_lchild();
				delete (*targetptr)->get_rchild();
				(*targetptr)->set_lchild(0);
				(*targetptr)->set_rchild(0);
				(*targetptr)->set_height(0);
			}
			else{
				node_t<T> * parA,* lchildA,* rchildA;
				node_t<T> * parB,* lchildB,* rchildB;
				node_t<T> * replacement;
				bool rightHeavy = false;

				parA	= (*targetptr)->get_parent();
				lchildA = (*targetptr)->get_lchild();
				rchildA = (*targetptr)->get_rchild();
				if(lheight <= rheight){			//The node is either balanced or right-heavy
					rightHeavy = true;
					replacement = (*targetptr)->get_rchild()->min_elt();

					delete replacement->get_lchild();	//Since this is the min elt, we delete its blank neighbor
					replacement->set_lchild(0);
				}
				else{						//It's left-heavy
					replacement = (*targetptr)->get_lchild()->max_elt();

					delete replacement->get_rchild();	//Since this is the max elt, we delete its blank neighbor
					replacement->set_rchild(0);
				}
				parB	= replacement->get_parent();
				lchildB = replacement->get_lchild();
				rchildB = replacement->get_rchild();
				//reference = parB;						//To remember where to balance from

				//Now patch in the replacement node:
				replacement->set_parent(parA);
				(*targetptr) = replacement;

				if((replacement == copy->get_lchild()) || (replacement == copy->get_rchild())){		//The special case where the replacement is directly ajacent to this node
					if(rightHeavy){
						replacement->set_lchild(lchildA);
						lchildA->set_parent(replacement);
					}
					else{
						replacement->set_rchild(rchildA);
						rchildA->set_parent(replacement);
					}
					parB = replacement;				//because at this point parB is off the tree, and we need to balance from where it left
				}
				else{
					replacement->set_lchild(lchildA);
					replacement->set_rchild(rchildA);
					rchildA->set_parent(replacement);
					lchildA->set_parent(replacement);

					if(!rightHeavy){				//If replacement was its parent's right child
						parB->set_rchild(lchildB);
						lchildB->set_parent(parB);
					}
					else{							//else it was its parent's left child
						parB->set_lchild(rchildB);
						rchildB->set_parent(parB);
					}
				}
				delete copy;
				parB->balance();
			}
		}
	}
	return 0;
}

template<class T>void bst_t<T>::export_df(vector<T> & result){
	root->export_df(result);
}

// Search algorithm (can be replaced in constructor):
	//By default it will return all elements that are equal to the search term
template<class T>int def_search(node_t<T> * targ,T obj,vector<T> & results,bool (*cmp)(const T,const T)){
	T temp;
	if(targ->key(temp) == NODE_EMPTY){
		return -1;
	}

	if(cmp(obj,temp)){
		return(def_search(targ->get_lchild(),obj,results,cmp));
	}
	else if(cmp(temp,obj)){
		return(def_search(targ->get_rchild(),obj,results,cmp));
	}
	else{			//They're equal
		vector<T> * tmpcontents = targ->get_contents();
		unsigned sz = targ->get_contents()->size();
		for (unsigned i = 0; i < sz; i++){
			results.push_back(tmpcontents->at(i));
		}
		return 0;
	}
}

template<class T>bst_t<T>::bst_t(){
	//WARNING: This constructor is only to be used if other classes inherit
		// from this class.  It just exists to make the compiler happy.
	root = new node_t<T>();
	search_alg = &(def_search<T>);
}

template<class T>bst_t<T>::bst_t(bool (*compare)(const T,const T)){
	root = new node_t<T>();
	cmp = compare;
	search_alg = &(def_search<T>);
}

template<class T>bst_t<T>::bst_t(bool (*compare)(const T,const T),int (*nsearch)(node_t<T> *,T,vector<T> &,bool (*)(const T, const T))){
	root = new node_t<T>();
	cmp = compare;
	search_alg = nsearch;
}

template<class T>bst_t<T>::~bst_t(){
	root->delete_children();
	delete root;
}

#endif
