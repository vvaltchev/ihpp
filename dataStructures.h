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
    virtual keyT getKey() const = 0;
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

        bool _valid;
        typename std::map< keyT, valueT >::iterator _it;

        iterator(typename std::map< keyT, valueT >::iterator const& baseIt)
            : _valid(true)
            , _it(baseIt)
        {  }

    public:

        iterator() : _valid(false) { }
        bool isValid() const { return _valid; }
        valueT &operator *() { return _it->second; }
        valueT *operator->() { return &_it->second; }

        void operator++() { ++_it; }
        void operator--() { --_it; }
        void operator++(int dummy) { _it++; }
        void operator--(int dummy) { _it--; }

        bool operator==(const iterator& _it2) const { return _it==_it2._it; }
        bool operator!=(const iterator& _it2) const { return _it!=_it2._it; }
        bool operator<(const iterator& _it2) const { return _it<_it2._it; }
        bool operator<=(const iterator& _it2) const { return _it<=_it2._it; }
        bool operator>(const iterator& _it2) const { return _it>_it2._it; }
        bool operator>=(const iterator& _it2) const { return _it>=_it2._it; }
    };

    class const_iterator {

        friend class ihppNodeChildrenContainerMap<keyT, valueT>;

        bool _valid;
        typename std::map< keyT, valueT >::const_iterator _it;

        const_iterator(typename std::map< keyT, valueT >::const_iterator const& baseIt)
            : _valid(true)
            , _it(baseIt)
        {  }

    public:

        const_iterator() : _valid(false) { }
        bool isValid() const { return _valid; }
        const valueT &operator *() { return _it->second; }
        const valueT *operator->() { return &_it->second; }

        void operator++() { ++_it; }
        void operator--() { --_it; }
        void operator++(int dummy) { _it++; }
        void operator--(int dummy) { _it--; }

        bool operator==(const const_iterator& _it2) const { return _it==_it2._it; }
        bool operator!=(const const_iterator& _it2) const { return _it!=_it2._it; }
        bool operator<(const const_iterator& _it2) const { return _it<_it2._it; }
        bool operator<=(const const_iterator& _it2) const { return _it<=_it2._it; }
        bool operator>(const const_iterator& _it2) const { return _it>_it2._it; }
        bool operator>=(const const_iterator& _it2) const { return _it>=_it2._it; }
    };

    iterator begin() { return _data.begin(); }
    iterator end() { return _data.end(); }
    const_iterator begin() const { return _data.begin(); }
    const_iterator end() const { return _data.end(); }
    const_iterator cbegin() const { return _data.begin(); }
    const_iterator cend() const { return _data.end(); }

    iterator find(const keyT& key) { return _data.find(key); }
    const_iterator find(const keyT& key) const { return _data.find(key); }
    size_t size() const { return _data.size(); }

    //no checks are made!
    valueT& insert(const keyT& key, const valueT& val) { return _data[key]=val; }
    valueT& replace(const keyT& key, const valueT& val) { return _data[key]=val; }
};



/*
    valueT is assumed to be an ObjectWithKey and to be NOT a pointer.
*/
template <typename keyT, typename valueT>
class ihppNodeChildrenContainer {

    std::list<valueT> _data;

public:

    typedef typename std::list<valueT>::iterator iterator;
    typedef typename std::list<valueT>::const_iterator const_iterator;

    auto size() const { return _data.size(); }
    auto begin() { return _data.begin(); }
    auto end() { return _data.end(); }
    auto begin() const { return const_iterator(_data.begin()); }
    auto end() const { return const_iterator(_data.end()); }
    auto cbegin() const { return const_iterator(_data.begin()); }
    auto cend() const { return const_iterator(_data.end()); }

    iterator find(const keyT& key) {

        iterator it;

        for (it = begin(); it != end(); it++)
            if (it->getKey() == key)
                return it;

        return end();
    }

    const_iterator find(const keyT& key) const {

        const_iterator it;

        for (it = cbegin(); it != cend(); it++)
            if (it->getKey() == key)
                return it;

        return cend();
    }

    valueT& insert(const keyT& key, const valueT& val) {

        _data.push_back(val);
        return _data.back();
    }

    valueT& replace(const keyT& key, const valueT& val) {

        iterator it = find(key);
        assert(it != end());
        return *it = val;
    }
};

