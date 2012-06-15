
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
#include "options.h"

#ifndef __KCCF_HEADER__
#define __KCCF_HEADER__

namespace kCCFLib {



class specialAttrs {

public:

#if defined(_WIN32)

	ADDRINT _NLG_Notify_addr;
	ADDRINT _NLG_Notify1_addr;
	ADDRINT __NLG_Dispatch_addr;

#endif

	specialAttrs() : _NLG_Notify_addr(0), _NLG_Notify1_addr(0), __NLG_Dispatch_addr(0) { }
};

enum WorkingModeType { FuncMode, BlockMode, TradMode };

class kCCFContextClass {

	PIN_LOCK lock;
	unsigned int _K_CCF_VAL;
	WorkingModeType _WorkingMode;

public:

	ofstream OutFile;

	set<string> funcsToTrace;

	BlocksMap allBlocks;
	FuncsMap allFuncs;
	vector<kCCFThreadContextClass*> threadContexts;

	bool exitPassed;
	
	optionsClass options;
	specialAttrs spAttrs;

	ADDRINT startFuncAddr;
	ADDRINT stopFuncAddr;


	
	kCCFContextClass(unsigned int k, WorkingModeType wm);
	~kCCFContextClass();

	kCCFThreadContextClass *getThreadCtx(PIN_THREAD_UID tid);
	
	WorkingModeType WorkingMode() { return _WorkingMode; }
	unsigned int kval() { return _K_CCF_VAL; }

	bool hasToTrace(string funcName, ADDRINT funcAddr);
};

#ifdef MAIN_KCCF_MODULE

kCCFContextClass *globalSharedContext=0;

#else

extern kCCFContextClass *globalSharedContext;

#endif


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


} //end namespace kCCFLib

#include "specialfuncs.h"

#endif