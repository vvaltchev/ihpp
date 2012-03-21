
#include <assert.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <stack>

#include "pin.H"

#include "benchmark.h"
#include "forest.h"
#include "tracingObjects.h"
#include "threadContext.h"
#include "specialfuncs.h"

#ifndef __KCCF_HEADER__
#define __KCCF_HEADER__

namespace kCCFLib {

class optionsClass {

public:

	bool joinThreads;
	bool rollLoops;

	bool showkSF;
	bool showkSF2;
	bool showkCCF;
	bool showFuncs;
	bool showBlocks;
	bool showCalls;
	bool purgeFuncs;
	bool disasm;

	optionsClass() 
	{
		joinThreads=false;
		rollLoops=false;
		showkSF=false;
		showkSF2=false;
		showkCCF=false;
		showFuncs=false;
		showBlocks=false;
		showCalls=false;
		purgeFuncs=false;	
		disasm=false;
	}
};

enum WorkingModeType { FuncMode, BlockMode, TradMode };

class kCCFContextClass : public optionsClass {

	PIN_LOCK lock;

public:

	ofstream OutFile;

	set<string> funcsToTrace;

	BlocksMap allBlocks;
	FuncsMap allFuncs;
	vector<kCCFThreadContextClass*> threadContexts;

	unsigned int K_CCF_VAL;
	WorkingModeType WorkingMode;
	

	bool exitPassed;
	
	
	string startFuncName;
	string stopFuncName;

	ADDRINT startFuncAddr;
	ADDRINT stopFuncAddr;
	
	kCCFContextClass(unsigned int k, WorkingModeType wm);
	~kCCFContextClass();

	kCCFThreadContextClass *getThreadCtx(PIN_THREAD_UID tid);

	bool hasToTrace(string funcName, ADDRINT funcAddr);
};

inline bool kCCFContextClass::hasToTrace(string funcName, ADDRINT funcAddr) 
{
	if (funcsToTrace.size()) 
		return funcsToTrace.find(funcName) != funcsToTrace.end();

	return allFuncs.find(funcAddr) != allFuncs.end();
}


template <typename keyT>
void kSlabForestKLevelCountersClear(forest<keyT> &f, keyT &rootKey, unsigned int k) {

	typename forest<keyT>::treesIterator it = f.getTreesIteratorBegin();
	typename forest<keyT>::treesIterator end = f.getTreesIteratorEnd();

	while (it != end) {
	
		if (it->getKey() != rootKey)
			it->clearLevelKCounters(k);

		it++;
	}
}



template <typename T>
void traceObject(TracingObject<T> *to, int k, 
											kCCFAbstractContext *ctx, kCCFNode* &treeTop, 
											kCCFNode* &treeBottom, bool accumulate=false) 
{

	kCCFNode *n;
	kCCFNodeMap::iterator it;
	ADDRINT key = to->getKey();


	if (!treeTop && !treeBottom) {
	
		ctx->rootKey=key;
	}

	/*
		When accumulate is true, consecutive runs of the same object, say A, 
		will increment it's counter instead of adding a node A as child of A.
		For example, with accumulate=true, this IS NOT possibile:
	
		A,1
		   | A,1
			  | A,1

		That calling sequence will became:
		A,3
	*/
	if (accumulate) {
	
		
		if (treeTop && to == treeTop->getValue()) {
	
			treeTop->incCounter();
			return;
		}

	}


	if (!k) {
	
		n = ctx->kSlabForest.getTreeRef(key);

		if (n) 
			n->incCounter();
		else 
			ctx->kSlabForest.addTreeByVal(kCCFNode(key,to,1));
		
		return;
	}

	// For k -> inf, this code runs only first time, when counter=0
	if (!( (ctx->counter) % k )) {

		treeBottom=treeTop;
		it = ctx->R.find(key);

		if (it == ctx->R.end())
			ctx->R[key] = ( treeTop = ctx->kSlabForest.addTreeByVal(kCCFNode(key, to, 0))  );
		else
			treeTop = it->second;
	
	} else {
		
		//For k -> inf, only this code runs

		n = treeTop->getChildRef(key);

		if (!n)
			n = treeTop->addChildByVal(kCCFNode(key, to, 0));
		
		treeTop=n;
	}
	

	treeTop->incCounter();

	//For k -> inf, this NEVER runs
	if (treeBottom) {
	
		n = treeBottom->getChildRef(key);

		if (!n)
			n = treeBottom->addChildByVal(kCCFNode(key, to, 0));
		
		treeBottom=n;
		treeBottom->incCounter();
	}

	ctx->counter++;
}

} //end namespace kCCFLib

#endif