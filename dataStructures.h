
#ifndef __DATA_STR_HEADER__
#define __DATA_STR_HEADER__

#include <vector>
#include <stack>
#include <map>


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



#endif