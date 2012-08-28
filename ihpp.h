
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

#ifndef __IHPP_HEADER__
#define __IHPP_HEADER__




class specialAttrs {

public:

#if defined(_WIN32)

	ADDRINT _NLG_Notify_addr;
	ADDRINT _NLG_Notify1_addr;
	ADDRINT __NLG_Dispatch_addr;
	ADDRINT __tmainCRTStartup_addr;
	ADDRINT wWinMain_addr;
	ADDRINT unnamedImageEntryPoint_addr;

#endif

	ADDRINT main_addr;
	ADDRINT exit_addr;

	ADDRINT text_addr;
	

	specialAttrs() :

#if defined(_WIN32)
					_NLG_Notify_addr(0), _NLG_Notify1_addr(0), 
					__NLG_Dispatch_addr(0), __tmainCRTStartup_addr(0), 
					wWinMain_addr(0), unnamedImageEntryPoint_addr(0), 
#endif
					main_addr(0), exit_addr(0), text_addr(0)
	{ }
};



class GlobalContext {

	PIN_LOCK lock;
	unsigned int _K_CCF_VAL;
	WorkingModeType _WorkingMode;

public:

	ofstream OutFile;

	set<string> funcsToTrace;
	set<ADDRINT> funcAddrsToTrace;

	BlocksMap allBlocks;
	FuncsMap allFuncs;
	vector<ThreadContext*> threadContexts;

	bool exitPassed;
	
#if DEBUG
	bool showDebug;
#endif

	optionsClass options;
	specialAttrs spAttrs;

	ADDRINT startFuncAddr;
	ADDRINT stopFuncAddr;


	
	GlobalContext(WorkingModeType wm, unsigned kval, optionsClass options);
	~GlobalContext();

	ThreadContext *getThreadCtx(PIN_THREAD_UID tid);
	
	WorkingModeType WorkingMode() { return _WorkingMode; }
	unsigned int kval() { return _K_CCF_VAL; }

	inline bool hasToTraceByName(string funcName, ADDRINT funcAddr);
	inline bool hasToTrace(ADDRINT funcAddr);
};

#ifdef MAIN_IHPP_MODULE

GlobalContext *globalSharedContext=0;

#else

extern GlobalContext *globalSharedContext;

#endif


inline bool GlobalContext::hasToTraceByName(string funcName, ADDRINT funcAddr) 
{
	if (funcsToTrace.size()) 
		return funcsToTrace.find(funcName) != funcsToTrace.end();

	return allFuncs.find(funcAddr) != allFuncs.end();
}


inline bool GlobalContext::hasToTrace(ADDRINT funcAddr)
{
	return funcAddrsToTrace.find(funcAddr) != funcAddrsToTrace.end();
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



#include "specialfuncs.h"

#endif