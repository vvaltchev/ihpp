
#include "specialfuncs.h"
#include "dataStructures.h"

#ifndef __INTRAMODE_CTX_HEADER__
#define __INTRAMODE_CTX_HEADER__

class ShadowStackItemType {

public:

	ihppNode *treeTop;
	ihppNode *treeBottom;

#if ENABLE_KEEP_STACK_PTR
	ADDRINT stackPtr;
#endif

#if ENABLE_INS_FORWARD_JMP_RECOGNITION
	unsigned int fjmps;
#endif

#if ENABLE_KEEP_STACK_PTR
	ShadowStackItemType(ihppNode *t, ihppNode *b, ADDRINT ptr) : treeTop(t), treeBottom(b), stackPtr(ptr) 
#else
	ShadowStackItemType(ihppNode *t, ihppNode *b) : treeTop(t), treeBottom(b)
#endif
	{ 

#if ENABLE_INS_FORWARD_JMP_RECOGNITION
		fjmps=0;
#endif

	}


};

class GenericTraceContext: public BenchmarkObj {

public:

	ADDRINT rootKey;
	ihppForest kSlabForest;
	unsigned int counter;

	ihppStack<ShadowStackItemType> shadowStack;

	GenericTraceContext() : rootKey(0), counter(0) 
	{ 
		BENCHMARK_INIT_VARS
	}

};

class IntraModeContext : public GenericTraceContext {

	ADDRINT funcAddr;

public:

	IntraModeContext(ADDRINT functionAddr) : GenericTraceContext(), funcAddr(functionAddr) { }

	ADDRINT getFunctionAddr() { return funcAddr; }
};

#endif