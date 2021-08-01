#pragma once
#include "node.h"

template <typename keyT>
class forest {

    ihppNodeChildrenContainerMap< keyT, node<keyT> > trees;
    static void joinSubtrees(node<keyT> &t1, const node<keyT> &t2);

public:

    static forest<keyT> join(node<keyT> &t1, node<keyT> &t2);
    static forest<keyT> join(forest<keyT> &f1, node<keyT> &t2);
    static forest<keyT> join(forest<keyT> &f1, forest<keyT> &f2);

    typedef typename ihppNodeChildrenContainerMap< keyT, node<keyT>  >::iterator iterator;

    forest() = default;
    forest(const forest &f);
    forest(const node<keyT>& n);

    node<keyT> *getTreeRef(keyT k);
    node<keyT> *toTree();
    node<keyT> *addTree(const node<keyT>& n);

    auto begin() { return trees.begin(); }
    auto end() { return trees.end(); }
    auto begin() const { return trees.begin(); }
    auto end() const { return trees.end(); }

    size_t treesCount() { return trees.size(); }
    size_t recursiveAllNodesCount();

    void autoSetParents();

    forest<keyT> inverseK(unsigned int k);

    void local_join(const node<keyT>& t2);
    void local_join(const forest<keyT>& f2);

    forest<keyT> &operator=(const forest<keyT>& f);
};


template <typename keyT>
inline forest<keyT>::forest(const forest<keyT>& f)
    : trees(f.trees)
{
    BM_inc_forests_copied();
}

template <typename keyT>
inline forest<keyT>::forest(const node<keyT>& n)
    : trees()
{
    addTree(n);
    BM_inc_forests_copied();
}

template <typename keyT>
inline forest<keyT> &forest<keyT>::operator=(const forest<keyT>& f)
{
    trees = f.trees;
    BM_inc_forests_copied();
    return *this;
}

template <typename keyT>
inline node<keyT>* forest<keyT>::addTree(const node<keyT>& n) {

    assert(!getTreeRef(n.getKey()));
    return &trees.insert(n.getKey(), n);
}

template <typename keyT>
size_t forest<keyT>::recursiveAllNodesCount() {

    size_t count = 0;

    for (auto& tree : *this)
        count += tree.recursiveAllNodesCount();

    return count;
}

template <typename keyT>
inline node<keyT>* forest<keyT>::getTreeRef(keyT k) {

    iterator it = trees.find(k);

    if (it != end())
        return &(*it);

    return 0;
}

template <typename keyT>
node<keyT>* forest<keyT>::toTree() {

    assert(treesCount() == 1);

    return &(*begin());
}

template <typename keyT>
void forest<keyT>::autoSetParents() {

    iterator it;

    for (it=begin(); it != end(); it++)
        if (!it->getParentRef())
            it->autoSetParents();
}


template <typename keyT>
void forest<keyT>::joinSubtrees(node<keyT> &t1, const node<keyT> &t2) {

    t1.setCounter(t1.getCounter() + t2.getCounter());

    for (const auto& t2_child : t2) {

        node<keyT> *t = t1.getChildRef(t2_child.getKey());

        if (!t)
            t1.addChild(t2_child);
        else
            joinSubtrees(*t, t2_child);
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
void forest<keyT>::local_join(const node<keyT>& t2) {

    node<keyT> *t = getTreeRef(t2.getKey());

    if (t)
        joinSubtrees(*t, t2);
    else
        addTree(t2);
}

template <typename keyT>
void forest<keyT>::local_join(const forest<keyT>& f2) {

    for (auto& tree : f2)
        local_join(tree);
}

template <typename keyT>
forest<keyT> forest<keyT>::join(forest<keyT> &f1, node<keyT> &t2) {

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

    forest<keyT> res;

    for (auto& tree : *this) {

        tree.autoSetParents();
        const vector< node<keyT> * >& tmp = tree.getAllTreeNodesRef();

        for (node<keyT> *other_tree : tmp)
            res.local_join(other_tree->kpathR(k));
    }

    return res;
}
