#include "../forest.h"
#include "../util.h"

template <typename keyT>
class TracingObject : public ObjectWithKey<keyT> {

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


class simpleVal : public ObjectWithKey<const char*> {

	const char *key;

public:
	simpleVal(const char *k) { key=k; }
	const char *getKey() { return key; }
};

template <typename T>
class ShadowStackItemType {

public:

	node<T> *treeTop;
	node<T> *treeBottom;
	
	ShadowStackItemType() : treeTop(0), treeBottom(0) { }
	ShadowStackItemType(node<T> *t, node<T> *b) : treeTop(t), treeBottom(b) { 	}
};

template<typename T>
class testCtx {

public:

	T rootKey;
	forest<T> kSlabForest;
	map<T, node<T> * > R;

	unsigned int counter;

	stack< ShadowStackItemType<T> > shadowstack;

	testCtx() { counter=0; }
};

class testFuncObj : public TracingObject<const char*> {

public:

	testFuncObj(const char *k) : TracingObject(k) { }
	operator string() const { return string(key)+"()"; }
};


