#pragma once

#include <map>
#include <stack>
#include <vector>
#include <iostream>

#include "dataStructures.h"
#include "benchmark.h"

using namespace std;


template <typename keyT>
class node : public ObjectWithKey<keyT> {

private:
    node<keyT> *parent;

protected:

    ObjectWithKey<keyT> *val;
    obj_counter_t counter;

    ihppNodeChildrenContainer< keyT, node<keyT> > children;

    void clearLevelKCounters(unsigned int k, unsigned int deepth);
    void getAllTreeNodesRef(vector< node<keyT>* > &vec);

public:

    typedef typename ihppNodeChildrenContainer< keyT, node<keyT> >::iterator iterator;

    node();
    node(const node& n);
    node(keyT key, ObjectWithKey<keyT>* val, obj_counter_t counter=0);
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
    ObjectWithKey<keyT> * getValue() { assert(isValid()); return val; }

    auto begin() { return children.begin(); }
    auto end() { return children.end(); }

    void incCounter() { counter++; }
    obj_counter_t getCounter() { return counter; }
    void setCounter(obj_counter_t c) { counter=c; }

    node<keyT> kpath(unsigned int k);
    node<keyT> kpathR(unsigned int k);

    void resetVisitedRecursive();
    inline void clearLevelKCounters(unsigned int k);

    operator bool() { return isValid(); }

    node<keyT> &operator=(node<keyT> n);

    //for this to work, typeof(*val) MUST BE castable to string
    operator string() { return (val) ? (string)*getValue() : string(); }
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
inline node<keyT>::node(keyT key, ObjectWithKey<keyT>* val, obj_counter_t counter) {

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
inline ostream& operator<<(ostream& s, node<keyT> n) {
    return n ? s << n.getKey() : s << "(null)";
}


template <typename keyT>
void node<keyT>::clearLevelKCounters(unsigned int k, unsigned int deepth) {

    iterator it;

    if (deepth >= k)
        return;

    counter=0;

    for (it = begin(); it != end(); it++)
        it->clearLevelKCounters(k, deepth+1);
}

template <typename keyT>
size_t node<keyT>::recursiveAllNodesCount() {

    iterator it;
    size_t count=1;

    for (it = begin(); it != end(); it++)
        count+=it->recursiveAllNodesCount();

    return count;
}

template <typename keyT>
inline void node<keyT>::clearLevelKCounters(unsigned int k) {

    clearLevelKCounters(k, 0);
}

template <typename keyT>
void node<keyT>::getAllTreeNodesRef(vector< node<keyT>* > &vec) {

    iterator it;

    vec.push_back(this);

    for (it = begin(); it != end(); it++)
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
    size_t i=0;

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

    iterator it;

    it = children.find(k);

    if (it != end())
        return &(*it);

    return 0;
}

template <typename keyT>
void node<keyT>::autoSetParents() {

    iterator it;

    for (it=begin(); it != end(); it++) {

        it->parent=this;
        it->autoSetParents();
    }
}
