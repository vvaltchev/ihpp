
#ifndef __FOREST_H__
#define __FOREST_H__

#include "node.h"

namespace kCCFLib { 

template <typename keyT, typename valueT>
class forest {

	kNodeChildrenContainer< keyT, node<keyT,valueT> > trees;
	static void _joinChildren(forest<keyT, valueT> &res, node<keyT, valueT> &t1, node<keyT, valueT> &t2);

public:
	
	static forest<keyT, valueT> join(node<keyT, valueT> &t1, node<keyT, valueT> &t2);
	static forest<keyT, valueT> join(forest<keyT, valueT> &f1, forest<keyT, valueT> &f2);
	static forest<keyT, valueT> join(forest<keyT, valueT> &f1, node<keyT, valueT> &t2);

	typedef typename kNodeChildrenContainer< keyT, node<keyT,valueT> >::iterator treesIterator;

	forest();
	forest(const forest &f);
	forest(node<keyT,valueT> n);

	node<keyT,valueT>* getTreeRef(keyT k);
	node<keyT,valueT>* toTree();

	inline node<keyT,valueT>* addTree(node<keyT,valueT> &n);
	node<keyT,valueT>* addTreeByVal(node<keyT,valueT> n) { return addTree(n); }

	treesIterator getTreesIteratorBegin() { return treesIterator(trees.begin()); }
	treesIterator getTreesIteratorEnd() { return treesIterator(trees.end()); }
	
	size_t treesCount() { return trees.size(); }
	size_t recursiveAllNodesCount();

	void autoSetParents();

	forest<keyT, valueT> inverseK(unsigned int k);
	forest<keyT, valueT> join(forest<keyT, valueT> &f2) { return join(*this, f2); }
	forest<keyT, valueT> joinByVal(forest<keyT, valueT> f2) { return join(*this, f2); }
	forest<keyT, valueT> join(node<keyT, valueT> &t2) { return join(*this, t2); }
	forest<keyT, valueT> joinByVal(node<keyT, valueT> t2) { return join(*this, t2); }

	inline forest<keyT,valueT> &operator=(forest<keyT,valueT> f);
};


template <typename keyT, typename valueT>
inline forest<keyT,valueT>::forest() { }

template <typename keyT, typename valueT>
inline forest<keyT,valueT>::forest(const forest<keyT,valueT> &f) { 

	trees = f.trees;
	BM_inc_forests_copied();
}

template <typename keyT, typename valueT>
inline forest<keyT,valueT>::forest(node<keyT,valueT> n) { 

	addTree(n); 

	BM_inc_forests_copied();
}

template <typename keyT, typename valueT>
inline forest<keyT,valueT> &forest<keyT,valueT>::operator=(forest<keyT,valueT> f) {

	trees=f.trees;

	BM_inc_forests_copied();

	return *this;
}


template <typename keyT, typename valueT>
inline node<keyT,valueT>* forest<keyT,valueT>::addTree(node<keyT,valueT> &n) { 

	assert(!getTreeRef(n.getKey()));
	
	return &trees.insert(n.getKey(), n); 
}

template <typename keyT, typename valueT>
size_t forest<keyT,valueT>::recursiveAllNodesCount() {

	treesIterator it;
	size_t count=0;

	for (it=getTreesIteratorBegin(); it != getTreesIteratorEnd(); it++)
		count+=it->recursiveAllNodesCount();

	return count;
}

template <typename keyT, typename valueT>
inline node<keyT,valueT>* forest<keyT,valueT>::getTreeRef(keyT k) {

	treesIterator it;

	it = trees.find(k);

	if (it != getTreesIteratorEnd())
		return &(*it);

	return 0;
}

template <typename keyT, typename valueT>
node<keyT,valueT>* forest<keyT, valueT>::toTree() {

	assert(treesCount() == 1);

	return &(*getTreesIteratorBegin());
}

template <typename keyT, typename valueT>
void forest<keyT, valueT>::autoSetParents() {

	treesIterator it;
	
	for (it=getTreesIteratorBegin(); it != getTreesIteratorEnd(); it++)
		if (!it->getParentRef())
			it->autoSetParents();
}



template <typename keyT, typename valueT>
void forest<keyT, valueT>::
	_joinChildren(forest<keyT, valueT> &res, node<keyT, valueT> &t1, node<keyT, valueT> &t2) {

	node<keyT, valueT> *root;
	node<keyT, valueT> *t2Node;

	typename node<keyT, valueT>::nodesIterator it;

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


template <typename keyT, typename valueT>
forest<keyT, valueT> forest<keyT, valueT>::join(node<keyT, valueT> &t1, node<keyT, valueT> &t2) {

	forest<keyT, valueT> res;
	node<keyT, valueT> *n;

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

template <typename keyT, typename valueT>
forest<keyT, valueT> forest<keyT, valueT>::join(forest<keyT, valueT> &f1, forest<keyT, valueT> &f2) {

	forest<keyT, valueT> res;
	typename forest<keyT, valueT>::treesIterator it;
	node<keyT, valueT> *n2;


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

template <typename keyT, typename valueT>
forest<keyT, valueT> forest<keyT, valueT>::join(forest<keyT, valueT> &f1, node<keyT, valueT> &t2) {

	forest<keyT, valueT> res;
	typename forest<keyT, valueT>::treesIterator it;

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

template <typename keyT, typename valueT>
forest<keyT, valueT> forest<keyT, valueT>::inverseK(unsigned int k) {

	treesIterator it;
	forest<keyT, valueT> res;
	vector< node<keyT, valueT>* > tmp;
	typename vector< node<keyT, valueT>* >::iterator it2;
	
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

}

#endif