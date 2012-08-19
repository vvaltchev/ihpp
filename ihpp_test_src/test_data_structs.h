#include "../forest.h"
#include "../util.h"

template <typename keyT>
class TracingObject : public ihppObjectWithKey<keyT> {

protected:

	unsigned int simpleCounter;
	keyT key;

public:

	TracingObject(keyT k) : simpleCounter(0), key(k) {}
	virtual keyT getKey() { return key; }
	virtual unsigned int getSimpleCounter() { return simpleCounter; }
	virtual void incSimpleCounter() { simpleCounter++; }

	virtual operator string() const = 0;
};


class simpleVal : public ihppObjectWithKey<const char*> {

	const char *key;

public:
	simpleVal(const char *k) { key=k; }
	const char *getKey() { return key; }
};

template<typename T>
class testCtx {

public:

	T rootKey;
	forest<T> kSlabForest;
	map<T, node<T> * > R;

	unsigned int counter;

	node<T> *top;
	node<T> *bottom;

	testCtx() { counter=0; top=0; bottom=0; }
};

class testFuncObj : public TracingObject<const char*> {

public:

	testFuncObj(const char *k) : TracingObject(k) { }
	operator string() const { return string(key)+"()"; }
};


