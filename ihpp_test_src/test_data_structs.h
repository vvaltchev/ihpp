#include "../forest.h"
#include "../util.h"

template <typename keyT>
class TracingObject : public ObjectWithKey<keyT> {

protected:

    unsigned int simpleCounter;
    keyT key;

public:

    TracingObject(keyT k) : simpleCounter(0), key(k) {}
    virtual keyT getKey() const { return key; }
    virtual unsigned int getSimpleCounter() const { return simpleCounter; }
    virtual void incSimpleCounter() { simpleCounter++; }

    virtual operator string() const = 0;
};


class simpleVal : public ObjectWithKey<const char*> {

    const char *key;

public:
    simpleVal(const char *k) : key(k) { }
    const char *getKey() const { return key; }
};

template <typename T>
class ShadowStackItemType {

public:

    node<T> *treeTop;
    node<T> *treeBottom;

    ShadowStackItemType()
        : treeTop(nullptr)
        , treeBottom(nullptr)
    { }

    ShadowStackItemType(node<T> *t, node<T> *b) : treeTop(t), treeBottom(b) {     }
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


