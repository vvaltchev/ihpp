
#ifndef __DATA_STR_HEADER__
#define __DATA_STR_HEADER__

#include <vector>
#include <stack>
#include <map>


/*
	sizeof(std::stack<ADDRINT>) is 24, on x86 arch
	sizeof(std::vector<ADDRINT>) is 16, on x86 arch
*/

template <typename T>
class kStack {

	std::vector<T> _data;

public:

	kStack() { }

	void push(T val) { _data.push_back(val); }
	void pop() { _data.pop_back(); }
	T top() { return _data.back(); }
	size_t size() { return _data.size(); }
};


template <typename keyT, typename valueT>
class kNodeChildrenContainer {

	std::map<keyT, valueT> _data;

public:

	class iterator {
	
		friend class kNodeChildrenContainer<keyT, valueT>;

		bool valid;

		typename map< keyT, valueT >::iterator _it;
		iterator(typename map< keyT, valueT >::iterator baseIt) { _it=baseIt; valid=true; }

	public:

		iterator() { valid=false; }
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
	};

	iterator begin() { return iterator(_data.begin()); }
	iterator end() { return iterator(_data.end()); }

	size_t size() { return _data.size(); }

	valueT &operator[] (keyT key) { return _data[key]; }


};

#endif