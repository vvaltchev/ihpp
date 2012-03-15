#ifndef __NODE_HEADER__
#define __NODE_HEADER__

#include <map>
#include <stack>
#include <vector>
#include <iostream>

using namespace std;

namespace kCCFLib { 


template <typename keyT, typename valueT>
class node {

private:
	node< keyT, valueT > *parent; 

protected:

	valueT val;
	keyT key;
	
	map< keyT, node<keyT,valueT> > children;
	unsigned int counter;
	bool valid;


	void clearLevelKCounters(unsigned int k, unsigned int deepth);
	void getAllTreeNodesRef(vector< node<keyT,valueT>* > &vec);

public:


	class nodesIterator {
	
		friend class node<keyT,valueT>;

		bool valid;

		typename map< keyT, node<keyT,valueT> >::iterator _it;
		nodesIterator(typename map< keyT, node<keyT,valueT> >::iterator baseIt) { _it=baseIt; valid=true; }

	public:

		nodesIterator() { valid=false; }
		node<keyT,valueT> &operator *() { return _it->second; }
		node<keyT,valueT> *operator->() { return &_it->second; }

		void operator++() { ++_it; }
		void operator--() { --_it; }
		void operator++(int dummy) { _it++; }
		void operator--(int dummy) { _it--; }

		bool operator==(nodesIterator _it2) { return _it==_it2._it; }
		bool operator!=(nodesIterator _it2) { return _it!=_it2._it; }
		bool operator<(nodesIterator _it2) { return _it<_it2._it; }
		bool operator<=(nodesIterator _it2) { return _it<=_it2._it; }
		bool operator>(nodesIterator _it2) { return _it>_it2._it; }
	};

	node(); 
	node(keyT key, valueT val, unsigned int counter=0);
	~node();
	
	node<keyT, valueT>* getChildRef(keyT k);
	node<keyT, valueT>* addChild(node<keyT,valueT> &n);
	node<keyT, valueT>* replaceChild(node<keyT,valueT> &n);
	node<keyT, valueT>* addChildByVal(node<keyT,valueT> n) { return addChild(n); }

	node<keyT, valueT>* getParentRef() { return (parent && *parent) ?  parent : 0; }

	inline vector< node<keyT,valueT>* > getAllTreeNodesRef();

	size_t childrenCount() { return children.size(); }
	size_t recursiveAllNodesCount();

	void autoSetParents();

	keyT getKey() { return key; }
	valueT getValue() { return val; }
	bool isValid() { return valid; }

	
	nodesIterator getNodesIteratorBegin() { return nodesIterator(children.begin()); }
	nodesIterator getNodesIteratorEnd() { return nodesIterator(children.end()); }	

	void incCounter() { counter++; }
	unsigned int getCounter() { return counter; }
	void setCounter(unsigned int c) { counter=c; }

	node<keyT, valueT> kpath(unsigned int k);
	node<keyT, valueT> kpathR(unsigned int k);

	inline void clearLevelKCounters(unsigned int k);

	operator bool() { return valid; }
	
	node<keyT,valueT> &operator=(node<keyT,valueT> n);

	//for this to work, valueT MUST BE a POINTER and typeof(*valueT) MUST BE castable to string
	operator string() { return (val && valid) ? (string)*getValue() : string(); }
};

template <typename keyT, typename valueT>
inline node<keyT,valueT> &node<keyT,valueT>::operator=(node<keyT,valueT> n) {
	
		
	val=n.val;
	key=n.key;
	children=n.children;
	valid=n.valid;
	counter=n.counter;
	parent=0;

	BM_inc_nodes_copied();
	
	return *this;
}

template <typename keyT, typename valueT>
inline node<keyT,valueT>::node() { 

	valid=false; 
	parent=0;

	BM_inc_empty_nodes_created(); 
}

template <typename keyT, typename valueT>
inline node<keyT,valueT>::node(keyT key, valueT val, unsigned int counter) {

	this->key=key;
	this->val=val;
	this->counter=counter;
	this->parent=0;
	valid=true;

	BM_inc_nodes_created();
}


template <typename keyT, typename valueT>
inline node<keyT,valueT>::~node() {

}

template <typename keyT, typename valueT>
inline ostream& operator << (ostream& s, node<keyT, valueT> n) { return n ? s << n.getKey() : s << "(null)"; }



template <typename keyT, typename valueT>
void node<keyT,valueT>::clearLevelKCounters(unsigned int k, unsigned int deepth) {

	nodesIterator it;

	if (deepth >= k)
		return;

	counter=0;

	for (it = getNodesIteratorBegin(); it != getNodesIteratorEnd(); it++)
		it->clearLevelKCounters(k, deepth+1);
}

template <typename keyT, typename valueT>
size_t node<keyT,valueT>::recursiveAllNodesCount() {

	nodesIterator it;
	size_t count=1;

	for (it = getNodesIteratorBegin(); it != getNodesIteratorEnd(); it++)
		count+=it->recursiveAllNodesCount();

	return count;
}

template <typename keyT, typename valueT>
inline void node<keyT,valueT>::clearLevelKCounters(unsigned int k) {

	clearLevelKCounters(k, 0);
}

template <typename keyT, typename valueT>
void node<keyT,valueT>::getAllTreeNodesRef(vector< node<keyT,valueT>* > &vec) {

	nodesIterator it;

	vec.push_back(this);
	
	for (it = getNodesIteratorBegin(); it != getNodesIteratorEnd(); it++)	
		it->getAllTreeNodesRef(vec);
}


template <typename keyT, typename valueT>
inline vector< node<keyT,valueT>* > node<keyT,valueT>::getAllTreeNodesRef() {

	vector< node* > res;
	getAllTreeNodesRef(res);
	
	return res;
}



template <typename keyT, typename valueT>
node<keyT, valueT> node<keyT,valueT>::kpathR(unsigned int k) {

	node res;
	node *curr,*ptr;
	unsigned int i=0;
	
	if (!k)
		return node(key,val,counter);

	res = node(key,val,counter);
	ptr=&res;

	curr=this->parent;

	while (curr && curr->valid && i < k) {
	
		ptr=ptr->addChildByVal(node(curr->key,curr->val,counter));
		curr=curr->parent;
		i++;
	}

	return res;
}

template <typename keyT, typename valueT>
node<keyT, valueT> node<keyT,valueT>::kpath(unsigned int k) {

	node res;
	node *curr,*ptr;
	unsigned int i=0;
	stack<node> list;

	if (!k)
		return node(key,val,counter);

	list.push(node(key,val,counter));

	curr=this->parent;

	while (curr && curr->valid && i < k) {
	
		list.push(node(curr->key,curr->val,counter));
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


template <typename keyT, typename valueT>
inline node<keyT,valueT>* node<keyT,valueT>::addChild(node<keyT,valueT> &n) {

	register node *t;

	assert(!getChildRef(n.getKey()));

	t = &(children[n.getKey()]=n);
	t->parent=this;

	return t;
}

template <typename keyT, typename valueT>
inline node<keyT, valueT>* node<keyT,valueT>::replaceChild(node<keyT,valueT> &n) {

	register node *t;

	assert(getChildRef(n.getKey()));

	t = &(children[n.getKey()]=n);
	t->parent=this;

	return t;	
}

template <typename keyT, typename valueT>
inline node<keyT, valueT>* node<keyT,valueT>::getChildRef(keyT k) {

	nodesIterator it;

	//Forced linear search
	for (it=getNodesIteratorBegin(); it != getNodesIteratorEnd(); it++)
		if (it->getKey() == k)
			return &(*it);

	return 0;
}

template <typename keyT, typename valueT>
void node<keyT,valueT>::autoSetParents() {

	nodesIterator it;

	for (it=getNodesIteratorBegin(); it != getNodesIteratorEnd(); it++) {
	
		it->parent=this;
		it->autoSetParents();
	}
}


}

#endif