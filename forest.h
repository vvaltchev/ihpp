
#ifndef __FOREST_H__
#define __FOREST_H__

#include "node.h"


template <typename keyT>
class forest {

	ihppNodeChildrenContainer< keyT, node<keyT> > trees;
	static void _joinChildren(forest<keyT> &res, node<keyT>  &t1, node<keyT>  &t2);

public:
	
	static forest<keyT> join(node<keyT>  &t1, node<keyT>  &t2);
	static forest<keyT> join(forest<keyT> &f1, forest<keyT> &f2);
	static forest<keyT> join(forest<keyT> &f1, node<keyT>  &t2);

	typedef typename ihppNodeChildrenContainer< keyT, node<keyT>  >::iterator treesIterator;

	forest();
	forest(const forest &f);
	forest(node<keyT> n);

	node<keyT> * getTreeRef(keyT k);
	node<keyT> * toTree();

	inline node<keyT> * addTree(node<keyT> &n);
	node<keyT> * addTreeByVal(node<keyT> n) { return addTree(n); }

	treesIterator getTreesIteratorBegin() { return treesIterator(trees.begin()); }
	treesIterator getTreesIteratorEnd() { return treesIterator(trees.end()); }
	
	size_t treesCount() { return trees.size(); }
	size_t recursiveAllNodesCount();

	void autoSetParents();

	forest<keyT> inverseK(unsigned int k);
	forest<keyT> join(forest<keyT> &f2) { return join(*this, f2); }
	forest<keyT> joinByVal(forest<keyT> f2) { return join(*this, f2); }
	forest<keyT> join(node<keyT> &t2) { return join(*this, t2); }
	forest<keyT> joinByVal(node<keyT> t2) { return join(*this, t2); }

	inline forest<keyT> &operator=(forest<keyT> f);
};


template <typename keyT>
inline forest<keyT>::forest() { }

template <typename keyT>
inline forest<keyT>::forest(const forest<keyT> &f) { 

	trees = f.trees;
	BM_inc_forests_copied();
}

template <typename keyT>
inline forest<keyT>::forest(node<keyT>  n) { 

	addTree(n); 

	BM_inc_forests_copied();
}

template <typename keyT>
inline forest<keyT> &forest<keyT>::operator=(forest<keyT> f) {

	trees=f.trees;

	BM_inc_forests_copied();

	return *this;
}


template <typename keyT>
inline node<keyT>* forest<keyT>::addTree(node<keyT>  &n) { 

	assert(!getTreeRef(n.getKey()));
	
	return &trees.insert(n.getKey(), n); 
}

template <typename keyT>
size_t forest<keyT>::recursiveAllNodesCount() {

	treesIterator it;
	size_t count=0;

	for (it=getTreesIteratorBegin(); it != getTreesIteratorEnd(); it++)
		count+=it->recursiveAllNodesCount();

	return count;
}

template <typename keyT>
inline node<keyT>* forest<keyT>::getTreeRef(keyT k) {

	treesIterator it;

	it = trees.find(k);

	if (it != getTreesIteratorEnd())
		return &(*it);

	return 0;
}

template <typename keyT>
node<keyT>* forest<keyT>::toTree() {

	assert(treesCount() == 1);

	return &(*getTreesIteratorBegin());
}

template <typename keyT>
void forest<keyT>::autoSetParents() {

	treesIterator it;
	
	for (it=getTreesIteratorBegin(); it != getTreesIteratorEnd(); it++)
		if (!it->getParentRef())
			it->autoSetParents();
}



template <typename keyT>
void forest<keyT>::
	_joinChildren(forest<keyT> &res, node<keyT>  &t1, node<keyT>  &t2) {

	node<keyT>  *root;
	node<keyT>  *t2Node;

	typename node<keyT> ::nodesIterator it;

	root = res.getTreeRef(t1.getKey());

	for (it = t1.getNodesIteratorBegin(); it != t1.getNodesIteratorEnd(); it++) {

		t2Node = t2.getChildRef(it->getKey());

		if (t2Node)
			root->replaceChild(*join(*it, *t2Node).toTree());
	}

	for (it = t2.getNodesIteratorBegin(); it != t2.getNodesIteratorEnd(); it++)
		if (!root->getChildRef(it->getKey()))
			root->addChild(*it);
}


template <typename keyT>
forest<keyT> forest<keyT>::join(node<keyT> &t1, node<keyT> &t2) {

	forest<keyT> res;
	node<keyT>  *n;

	if (t1.getKey() != t2.getKey()) {
	
		res.addTree(t1);
		res.addTree(t2);
		return res;
	}

	n=res.addTree(t1);
	
	n->setCounter(t1.getCounter()+t2.getCounter());

	if (!t1.childrenCount() && !t2.childrenCount())
		return res;

	_joinChildren(res, *n, t2);

	return res;
}

template <typename keyT>
forest<keyT> forest<keyT>::join(forest<keyT> &f1, forest<keyT> &f2) {

	forest<keyT> res;
	typename forest<keyT>::treesIterator it;
	node<keyT>  *n2;


	for (it=f1.getTreesIteratorBegin(); it != f1.getTreesIteratorEnd(); it++) {
	
		n2 = f2.getTreeRef(it->getKey());

		if (!n2) {
		
			res.addTree(*it);
			continue;
		}

		res.addTree(*join(*it, *n2).toTree());
	}


	for (it=f2.trees.begin(); it != f2.trees.end(); it++) {
	
		if (it->getParentRef())
			continue;

		if (res.getTreeRef(it->getKey()))
			continue;

		res.addTree(*it);
	}

	return res;
}

template <typename keyT>
forest<keyT> forest<keyT>::join(forest<keyT> &f1, node<keyT>  &t2) {

	forest<keyT> res;
	typename forest<keyT>::treesIterator it;

	for (it=f1.getTreesIteratorBegin(); it != f1.getTreesIteratorEnd(); it++) {
	
		if (it->getKey() == t2.getKey()) {
	
			res.addTree(*join(*it, t2).toTree());
			continue;
		} 
		
		res.addTree(*it);
	}

	if (!res.getTreeRef(t2.getKey()))
		res.addTree(t2);

	return res;
}

template <typename keyT>
forest<keyT> forest<keyT>::inverseK(unsigned int k) {

	treesIterator it;
	forest<keyT> res;
	vector< node<keyT> * > tmp;
	typename vector< node<keyT> * >::iterator it2;
	
	size_t i=1;
	size_t count = recursiveAllNodesCount();
	bool showInfo=false;

	if (count > 100)
		showInfo=true;

	if (showInfo)
		cerr << "Warning: computing inverseK of forest with " << count << " nodes: it could take a while..." << endl;

	for (it = getTreesIteratorBegin(); it != getTreesIteratorEnd(); it++) {
	
		if (showInfo && treesCount() > 1)
			cerr << "join path " << i++ << " of " << treesCount() << endl;

		it->autoSetParents();
		
		tmp = it->getAllTreeNodesRef();

		for (it2 = tmp.begin(); it2 != tmp.end(); it2++)
			res = res.joinByVal((*it2)->kpathR(k));
	}

	return res;
}


#endif