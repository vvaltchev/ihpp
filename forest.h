
#ifndef __FOREST_H__
#define __FOREST_H__

#include "node.h"


template <typename keyT>
class forest {

	ihppNodeChildrenContainerMap< keyT, node<keyT> > trees;
	static void joinSubtrees(node<keyT> &t1, node<keyT> &t2);

public:

	static forest<keyT> join(node<keyT> &t1, node<keyT> &t2);
	static forest<keyT> join(forest<keyT> &f1, node<keyT> &t2);
	static forest<keyT> join(forest<keyT> &f1, forest<keyT> &f2);
	
	typedef typename ihppNodeChildrenContainerMap< keyT, node<keyT>  >::iterator treesIterator;

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

	void local_join(node<keyT> &t2);
	void local_join(forest<keyT> &f2);

	void local_joinByVal(node<keyT> t2) { local_join(t2); }
	void local_joinByVal(forest<keyT> f2) { local_join(f2); }
	
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
void forest<keyT>::joinSubtrees(node<keyT> &t1, node<keyT> &t2) {

	node<keyT> *t;
	typename node<keyT>::nodesIterator it;

	t1.setCounter(t1.getCounter() + t2.getCounter());

	for (it = t2.getNodesIteratorBegin(); it != t2.getNodesIteratorEnd(); it++) {
	
		t = t1.getChildRef(it->getKey());

		if (!t)
			t1.addChild(*it);
		else
			joinSubtrees(*t, *it); 
	}
}

template <typename keyT>
forest<keyT> forest<keyT>::join(node<keyT> &t1, node<keyT> &t2) {

	forest<keyT> res;
	node<keyT> *n1;

	if (t1.getKey() != t2.getKey()) {
	
		res.addTree(t1);
		res.addTree(t2);
		return res;
	}

	n1 = res.addTree(t1);

	joinSubtrees(*n1, t2);

	return res;
}

template <typename keyT>
void forest<keyT>::local_join(node<keyT> &t2) {

	node<keyT> *t;

	t = getTreeRef(t2.getKey());

	if (t)
		joinSubtrees(*t, t2);
	else
		addTree(t2);

}

template <typename keyT>
void forest<keyT>::local_join(forest<keyT> &f2) {

	typename forest<keyT>::treesIterator it;

	for (it=f2.getTreesIteratorBegin(); it != f2.getTreesIteratorEnd(); it++)
		local_join(*it);
}


template <typename keyT>
forest<keyT> forest<keyT>::join(forest<keyT> &f1, node<keyT>  &t2) {

	forest<keyT> res=f1;
	res.local_join(t2);
	return res;
}


template <typename keyT>
forest<keyT> forest<keyT>::join(forest<keyT> &f1, forest<keyT> &f2) {

	forest<keyT> res = f1;
	res.local_join(f2);
	return res;
}


template <typename keyT>
forest<keyT> forest<keyT>::inverseK(unsigned int k) {

	treesIterator it;
	forest<keyT> res;
	vector< node<keyT> * > tmp;
	typename vector< node<keyT> * >::iterator it2;
	
	for (it = getTreesIteratorBegin(); it != getTreesIteratorEnd(); it++) {
	
		it->autoSetParents();
		
		tmp = it->getAllTreeNodesRef();

		for (it2 = tmp.begin(); it2 != tmp.end(); it2++)
			res.local_joinByVal((*it2)->kpathR(k));
	}

	return res;
}


#endif
