#pragma once
#include "node.h"

template <typename keyT>
class forest {

    static void joinSubtrees(node<keyT> &t1, const node<keyT> &t2);

    ihppNodeChildrenContainerMap< keyT, node<keyT> > trees;

public:

    typedef typename ihppNodeChildrenContainerMap< keyT, node<keyT>  >::iterator iterator;

    forest() = default;
    forest(const forest &f);
    forest(const node<keyT>& n);
    forest<keyT> &operator=(const forest<keyT>& f);

    node<keyT> *getTreeRef(keyT k);
    node<keyT> *toTree();
    node<keyT> *addTree(const node<keyT>& n);

    auto begin() { return trees.begin(); }
    auto end() { return trees.end(); }
    auto begin() const { return trees.begin(); }
    auto end() const { return trees.end(); }
    auto cbegin() { return trees.cbegin(); }
    auto cend() { return trees.cend(); }

    size_t treesCount() const { return trees.size(); }
    size_t recursiveAllNodesCount() const;

    void autoSetParents();

    forest<keyT> inverseK(unsigned int k);

    void join(const node<keyT>& t2);
    void join(const forest<keyT>& f2);
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
size_t forest<keyT>::recursiveAllNodesCount() const {

    size_t count = 0;

    for (const auto& tree : *this)
        count += tree.recursiveAllNodesCount();

    return count;
}

template <typename keyT>
inline node<keyT> *forest<keyT>::getTreeRef(keyT k) {

    auto it = trees.find(k);

    if (it != end())
        return &(*it);

    return 0;
}

template <typename keyT>
node<keyT>* forest<keyT>::toTree()
{
    assert(treesCount() == 1);
    return &(*begin());
}

template <typename keyT>
void forest<keyT>::autoSetParents()
{
    for (auto& child : *this) {
        if (!child.getParentRef())
            child.autoSetParents();
    }
}

template <typename keyT>
void forest<keyT>::joinSubtrees(node<keyT> &t1, const node<keyT> &t2)
{
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
void forest<keyT>::join(const node<keyT>& t2)
{
    node<keyT> *t = getTreeRef(t2.getKey());

    if (t)
        joinSubtrees(*t, t2);
    else
        addTree(t2);
}

template <typename keyT>
void forest<keyT>::join(const forest<keyT>& f2)
{
    for (auto& tree : f2)
        join(tree);
}

template <typename keyT>
forest<keyT> forest<keyT>::inverseK(unsigned int k)
{
    forest res;

    for (auto& tree : *this) {

        std::vector< node<keyT> * > nodes;

        tree.autoSetParents();
        tree.getAllTreeNodesRef(nodes);

        for (node<keyT> *other_tree : nodes)
            res.join(other_tree->kpathR(k));
    }

    return res;
}
