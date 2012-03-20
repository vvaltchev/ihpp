#ifndef __NODE_HEADER__
#define __NODE_HEADER__

#include <map>
#include <stack>
#include <vector>
#include <iostream>

#include "dataStructures.h"


using namespace std;

namespace kCCFLib { 


template <typename keyT>
class node : public kObjectWithKey<keyT> {

private:
	node<keyT> *parent; 

protected:

	kObjectWithKey<keyT> *val;
	size_t counter;

	kNodeChildrenContainer< keyT, node<keyT> > children;
	
	void clearLevelKCounters(unsigned int k, unsigned int deepth);
	void getAllTreeNodesRef(vector< node<keyT>* > &vec);

public:


	typedef typename kNodeChildrenContainer<keyT, node<keyT>>::iterator nodesIterator;

	node(); 
	node(const node& n);
	node(keyT key, kObjectWithKey * val, unsigned int counter=0);
	~node();
	
	node<keyT>* getChildRef(keyT k);
	node<keyT>* addChild(node<keyT> &n);
	node<keyT>* replaceChild(node<keyT> &n);
	node<keyT>* addChildByVal(node<keyT> n) { return addChild(n); }

	node<keyT>* getParentRef() { return (parent && *parent) ?  parent : 0; }

	inline vector< node<keyT>* > getAllTreeNodesRef();

	size_t childrenCount() { return children.size(); }
	size_t recursiveAllNodesCount();

	void autoSetParents();

	bool isValid() { return val != 0; }

	keyT getKey() { assert(isValid()); return val->getKey(); }
	kObjectWithKey * getValue() { assert(isValid()); return val; }
	
	nodesIterator getNodesIteratorBegin() { return nodesIterator(children.begin()); }
	nodesIterator getNodesIteratorEnd() { return nodesIterator(children.end()); }	

	void incCounter() { counter++; }
	unsigned int getCounter() { return counter; }
	void setCounter(unsigned int c) { counter=c; }

	node<keyT> kpath(unsigned int k);
	node<keyT> kpathR(unsigned int k);

	inline void clearLevelKCounters(unsigned int k);

	operator bool() { return isValid(); }
	
	node<keyT> &operator=(node<keyT> n);

	//for this to work, typeof(*val) MUST BE castable to string
	operator string() { return (val && valid) ? (string)*getValue() : string(); }
};

template <typename keyT>
inline node<keyT> &node<keyT>::operator=(node<keyT> n) {
	
		
	val=n.val;
	children=n.children;
	counter=n.counter;
	parent=0;

	BM_inc_nodes_copied();
	
	return *this;
}

template <typename keyT>
inline node<keyT>::node(const node<keyT> &n) {
	
		
	val=n.val;
	children=n.children;
	counter=n.counter;
	parent=0;

	BM_inc_nodes_copied();
}


template <typename keyT>
inline node<keyT>::node() { 

	val=0;
	parent=0;

	BM_inc_empty_nodes_created(); 
}

template <typename keyT>
inline node<keyT>::node(keyT key, kObjectWithKey* val, unsigned int counter) {

	this->val=val;
	this->counter=counter;
	this->parent=0;

	assert(val);
	assert(key == val->getKey());

	BM_inc_nodes_created();
}


template <typename keyT>
inline node<keyT>::~node() { }

template <typename keyT>
inline ostream& operator << (ostream& s, node<keyT> n) { return n ? s << n.getKey() : s << "(null)"; }


template <typename keyT>
void node<keyT>::clearLevelKCounters(unsigned int k, unsigned int deepth) {

	nodesIterator it;

	if (deepth >= k)
		return;

	counter=0;

	for (it = getNodesIteratorBegin(); it != getNodesIteratorEnd(); it++)
		it->clearLevelKCounters(k, deepth+1);
}

template <typename keyT>
size_t node<keyT>::recursiveAllNodesCount() {

	nodesIterator it;
	size_t count=1;

	for (it = getNodesIteratorBegin(); it != getNodesIteratorEnd(); it++)
		count+=it->recursiveAllNodesCount();

	return count;
}

template <typename keyT>
inline void node<keyT>::clearLevelKCounters(unsigned int k) {

	clearLevelKCounters(k, 0);
}

template <typename keyT>
void node<keyT>::getAllTreeNodesRef(vector< node<keyT>* > &vec) {

	nodesIterator it;

	vec.push_back(this);
	
	for (it = getNodesIteratorBegin(); it != getNodesIteratorEnd(); it++)	
		it->getAllTreeNodesRef(vec);
}


template <typename keyT>
inline vector< node<keyT>* > node<keyT>::getAllTreeNodesRef() {

	vector< node* > res;
	getAllTreeNodesRef(res);
	
	return res;
}



template <typename keyT>
node<keyT> node<keyT>::kpathR(unsigned int k) {

	node res;
	node *curr,*ptr;
	unsigned int i=0;
	
	if (!k)
		return node(getKey(),val,counter);

	res = node(getKey(),val,counter);
	ptr=&res;

	curr=this->parent;

	while (curr && curr->isValid() && i < k) {
	
		ptr=ptr->addChildByVal(node(curr->getKey(),curr->val,counter));
		curr=curr->parent;
		i++;
	}

	return res;
}

template <typename keyT>
node<keyT> node<keyT>::kpath(unsigned int k) {

	node res;
	node *curr,*ptr;
	unsigned int i=0;
	stack<node> list;

	if (!k)
		return node(getKey(),val,counter);

	list.push(node(getKey(),val,counter));

	curr=this->parent;

	while (curr && curr->isValid() && i < k) {
	
		list.push(node(curr->getKey(),curr->val,counter));
		curr=curr->parent;
		i++;
	}


	res = list.top();
	list.pop();

	ptr=&res;

	while (!list.empty()) {
		
		ptr=ptr->addChildByVal(list.top());
		list.pop();
	}
	return res;
}


template <typename keyT>
inline node<keyT>* node<keyT>::addChild(node<keyT> &n) {

	register node *t;

	assert(!getChildRef(n.getKey()));

	t = &children.insert(n.getKey(), n);
	t->parent=this;

	return t;
}

template <typename keyT>
inline node<keyT>* node<keyT>::replaceChild(node<keyT> &n) {

	register node *t;

	assert(getChildRef(n.getKey()));

	t = &children.replace(n.getKey(), n);
	t->parent=this;

	return t;	
}

template <typename keyT>
inline node<keyT>* node<keyT>::getChildRef(keyT k) {

	nodesIterator it;

	it = children.find(k);
	
	if (it != getNodesIteratorEnd())
		return &(*it);

	return 0;
}

template <typename keyT>
void node<keyT>::autoSetParents() {

	nodesIterator it;

	for (it=getNodesIteratorBegin(); it != getNodesIteratorEnd(); it++) {
	
		it->parent=this;
		it->autoSetParents();
	}
}


}

#endif