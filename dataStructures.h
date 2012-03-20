
#ifndef __DATA_STR_HEADER__
#define __DATA_STR_HEADER__

#include <vector>
#include <stack>
#include <map>
#include <list>
#include <assert.h>

/* 
	Abstract class (also called 'interface'): 
	node<keyT> and TracingObject<keyT> implement this interface
*/
template <typename keyT>
class kObjectWithKey {

public:

	virtual keyT getKey()=0;
};



template <typename T>
class kStack {

	std::vector<T> _data;

public:

	kStack() { }

	void push(T val) { _data.push_back(val); }
	void pop() { _data.pop_back(); }
	T top() { return _data.back(); }
	T top(size_t i) { return _data.at(_data.size()-1-i); }
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
	valueT is assumed to be an kObjectWithKey and to be NOT a pointer.
*/
template <typename keyT, typename valueT>
class kNodeChildrenContainerList1 {

	std::list<valueT> _data;

public:

	class iterator {
	
		friend class kNodeChildrenContainerList1<keyT, valueT>;

		bool valid;

		typename std::list< valueT >::iterator _it;
		iterator(typename std::list< valueT >::iterator baseIt) { _it=baseIt; valid=true; }

	public:

		iterator() { valid=false; }
		bool isValid() { return valid; }
		valueT &operator *() { return *_it; }
		valueT *operator->() { return &(*_it); }

		void operator++() { ++_it; }
		void operator--() { --_it; }
		void operator++(int dummy) {  _it++; }
		void operator--(int dummy) {  _it--; }

		bool operator==(iterator _it2) { return _it==_it2._it; }
		bool operator!=(iterator _it2) { return _it!=_it2._it; }
		bool operator<(iterator _it2) {  return _it<_it2._it; }
		bool operator>(iterator _it2) { return _it>_it2._it; }
		bool operator<=(iterator _it2) { return _it <= _it2._it; }
		bool operator>=(iterator _it2) { return _it >= _it2._it; }
	};

	size_t size() { return _data.size(); }
	iterator begin() { return iterator(_data.begin()); }
	iterator end() { return iterator(_data.end()); }
	
	iterator find(keyT key) { 
		
		iterator it;

		for (it = begin(); it != end(); it++) 
			if (it->getKey() == key)
				return it;

		return end();
	}


	valueT& insert(keyT key, valueT val) { _data.push_back(val); return _data.back(); }
	valueT& replace(keyT key, valueT val) { iterator it = find(key); assert(it != end()); return *it = val; }
};


/*
	valueT is assumed to be an kObjectWithKey and to be NOT a pointer.
*/
template <typename keyT, typename valueT>
class kNodeChildrenContainerList2 {

	/*
		Using a level of indirection for _data saves space when list is empty
	*/
	std::list<valueT> *_data;

public:

	class iterator {
	
		friend class kNodeChildrenContainerList2<keyT, valueT>;

		bool valid;

		typename std::list< valueT >::iterator _it;
		iterator(typename std::list< valueT >::iterator baseIt) { _it=baseIt; valid=true; }

	public:

		iterator() { valid=false; }
		bool isValid() { return valid; }
		valueT &operator *() { assert(valid); return *_it; }
		valueT *operator->() { assert(valid); return &(*_it); }

		void operator++() { assert(valid); ++_it; }
		void operator--() { assert(valid); --_it; }
		void operator++(int dummy) { assert(valid); _it++; }
		void operator--(int dummy) { assert(valid); _it--; }

		bool operator==(iterator _it2) { if (!valid && !_it2.valid) return true; if (valid != _it2.valid) return false; return _it==_it2._it; }
		bool operator!=(iterator _it2) { return !operator==(_it2); }
		bool operator<(iterator _it2) { if (!(valid && _it2.valid)) return false; return _it<_it2._it; }
		bool operator>(iterator _it2) { if (!(valid && _it2.valid)) return false; return _it>_it2._it; }
		bool operator<=(iterator _it2) { return *this < _it2 || *this == _it2; }
		bool operator>=(iterator _it2) { return *this > _it2 || *this == _it2; }
	};

	kNodeChildrenContainerList2() { 
	
		_data=0;
	}
	~kNodeChildrenContainerList2() { 
	
		if (_data)
			delete _data; 
	}

	
	kNodeChildrenContainerList2(const kNodeChildrenContainerList2 &c) {
	
		if (c._data) {

			_data = new std::list<valueT>();
			*_data=*c._data;
		
		} else {	
			_data=0;
		}
	}
	
	kNodeChildrenContainerList2<keyT,valueT>& operator=(kNodeChildrenContainerList2<keyT,valueT> c) {
		

		if (!c._data) {
		
			if (_data) delete _data;
			_data=0;
			return *this;
		}

		if (!_data) 
			_data = new std::list<valueT>();

		*_data=*c._data;
		return *this;
	}

	size_t size() { return _data ? _data->size() : 0; }
	iterator begin() { return _data ? iterator(_data->begin()) : iterator(); }
	iterator end() { return _data ? iterator(_data->end()) : iterator(); }
	
	iterator find(keyT key) { 
		
		iterator it;

		if (!_data)
			return end();

		for (it = begin(); it != end(); it++) 
			if (it->getKey() == key)
				return it;

		return end();
	}


	valueT& insert(keyT key, valueT val) { 
		
		if (!_data)
			_data = new std::list<valueT>();

		_data->push_back(val); 
		return _data->back(); 
	}

	valueT& replace(keyT key, valueT val) { 
	
		assert(_data);

		iterator it = find(key);

		assert(it != end());

		return *it = val;
	}

};

#define kNodeChildrenContainer kNodeChildrenContainerList1

#endif