
#ifndef __DATA_STR_HEADER__
#define __DATA_STR_HEADER__

#include <vector>
#include <stack>
#include <map>

//Abstract class

template <typename keyT>
class kObjectWithKey {

public:

	virtual keyT getKey()=0;
};




template <typename T>
class kStack {

/*
	sizeof(std::stack<ADDRINT>) is 24, on x86 arch
	sizeof(std::vector<ADDRINT>) is 16, on x86 arch
*/
	std::vector<T> _data;

public:

	kStack() { }

	void push(T val) { _data.push_back(val); }
	void pop() { _data.pop_back(); }
	T top() { return _data.back(); }
	size_t size() { return _data.size(); }
};


template <typename keyT, typename valueT>
class kNodeChildrenContainerMap {

	std::map<keyT, valueT> _data;

public:

	class iterator {
	
		friend class kNodeChildrenContainerMap<keyT, valueT>;

		bool valid;

		typename std::map< keyT, valueT >::iterator _it;
		iterator(typename std::map< keyT, valueT >::iterator baseIt) { _it=baseIt; valid=true; }

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
	iterator find(keyT key) { return _data.find(key); }

	size_t size() { return _data.size(); }

	//no checks are made!
	valueT& insert(keyT key, valueT val) { return _data[key]=val; }
	valueT& replace(keyT key, valueT val) { return _data[key]=val; }

};


/*
	valueT is assumed to be an kObjectWithKey
	and valueT is assumed to be NOT a pointer
*/
template <typename keyT, typename valueT>
class kNodeChildrenContainerVec {

	std::vector<valueT> _data;

public:

	class iterator {
	
		friend class kNodeChildrenContainerVec<keyT, valueT>;

		bool valid;

		typename std::vector< valueT >::iterator _it;
		iterator(typename std::vector< valueT >::iterator baseIt) { _it=baseIt; valid=true; }

	public:

		iterator() { valid=false; }
		valueT &operator *() { return *_it; }
		valueT *operator->() { return &(*_it); }

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
	
	iterator find(keyT key) { 
		
		for (iterator it = begin(); it != end(); it++) {
		
			valueT val = *it;
			kObjectWithKey<keyT> *objk = dynamic_cast< kObjectWithKey<keyT> * >(&val);

			assert(objk);

			if (objk->getKey() == key)
				return it;
		}

		return end();
	}

	size_t size() { return _data.size(); }
	
	valueT& insert(keyT key, valueT val) { _data.push_back(val); return _data.back(); }
	valueT& replace(keyT key, valueT val) { 
	
		iterator it = find(key);

		assert(it != end());

		*it = val;

		return *it;
	}
};


#define kNodeChildrenContainer kNodeChildrenContainerVec

#endif