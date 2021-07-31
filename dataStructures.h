#pragma once

#include <vector>
#include <stack>
#include <map>
#include <list>
#include <assert.h>
#include <stdint.h>

#include "config.h"

#if USE_ALWAYS_64BIT_COUNTER
    typedef uint64_t obj_counter_t;
#else
    typedef size_t obj_counter_t;
#endif

/*
    Abstract class (also called 'interface'):
    node<keyT> and TracingObject<keyT> implement this interface
*/
template <typename keyT>
class ObjectWithKey {

public:

    virtual keyT getKey() = 0;
};



template <typename T>
class ihppStack {

    std::vector<T> _data;

public:

    ihppStack() { }

    void push(T val) { _data.push_back(val); }
    void pop() { _data.pop_back(); }
    T& top() { return _data.back(); }
    T& top(size_t i) { return _data.at(_data.size()-1-i); }
    size_t size() { return _data.size(); }
};


template <typename keyT, typename valueT>
class ihppNodeChildrenContainerMap {

    std::map<keyT, valueT> _data;

public:

    class iterator {

        friend class ihppNodeChildrenContainerMap<keyT, valueT>;

        bool valid;

        typename std::map< keyT, valueT >::iterator _it;
        iterator(typename std::map< keyT, valueT >::iterator baseIt) { _it=baseIt; valid=true; }

    public:

        iterator() { valid=false; }
        bool isValid() { return valid; }
        valueT &operator *() { return _it->second; }
        valueT *operator->() { return &_it->second; }

        void operator++() { ++_it; }
        void operator--() { --_it; }
        void operator++(int dummy) { _it++; }
        void operator--(int dummy) { _it--; }

        bool operator==(iterator _it2) { return _it==_it2._it; }
        bool operator!=(iterator _it2) { return _it!=_it2._it; }
        bool operator<(iterator _it2) { return _it<_it2._it; }
        bool operator<=(iterator _it2) { return _it<=_it2._it; }
        bool operator>(iterator _it2) { return _it>_it2._it; }
        bool operator>=(iterator _it2) { return _it>=_it2._it; }
    };

    iterator begin() { return iterator(_data.begin()); }
    iterator end() { return iterator(_data.end()); }
    iterator find(keyT key) { return _data.find(key); }

    size_t size() { return _data.size(); }

    //no checks are made!
    valueT& insert(keyT key, valueT val) { return _data[key]=val; }
    valueT& replace(keyT key, valueT val) { return _data[key]=val; }
};



/*
    valueT is assumed to be an ObjectWithKey and to be NOT a pointer.
*/
template <typename keyT, typename valueT>
class ihppNodeChildrenContainer {

    std::list<valueT> _data;

public:

    typedef typename std::list<valueT>::iterator iterator;

    size_t size() const { return _data.size(); }
    iterator begin() { return iterator(_data.begin()); }
    iterator end() { return iterator(_data.end()); }

    iterator find(keyT key) {

        iterator it;

        for (it = begin(); it != end(); it++)
            if (it->getKey() == key)
                return it;

        return end();
    }

    valueT& insert(keyT key, valueT val) {

        _data.push_back(val);
        return _data.back();
    }

    valueT& replace(keyT key, valueT val) {

        iterator it = find(key);
        assert(it != end());
        return *it = val;
    }
};

