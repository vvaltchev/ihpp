
#include "config.h"
#include "debug.h"
#include "ihpp.h"

using namespace std;

#include "benchmark.h"
#include "output.h"

#include "tracingFuncs.h"


	
string ThreadContext::getCurrentFunctionName() { 
	return globalSharedContext->allFuncs[currentFunction]->functionName(); 
}

ThreadContext *GlobalContext::getThreadCtx(PIN_THREAD_UID tid) { 
	
	ThreadContext *ret;

	GetLock(&lock, 1);

	for (unsigned i=0; i < threadContexts.size(); i++) {
		
		if (threadContexts[i]->getThreadID() == tid) {
			
			ret = threadContexts[i];
			ReleaseLock(&lock);

			return ret;
		}
	}

	threadContexts.push_back(new ThreadContext(tid, startFuncAddr, stopFuncAddr));

	ret = threadContexts.back();
	ReleaseLock(&lock);

	if (globalSharedContext->funcsToTrace.size()) {
	
		ihppNode *t=0,*b=0;
		
		if (globalSharedContext->WorkingMode() != WM_InterProcMode) {

			traceObject(globalSharedContext->allFuncs[(ADDRINT)-1], ret, t, b);
			ret->shadowStack.push(ShadowStackItemType(t,b,(ADDRINT)-1));

			if (globalSharedContext->WorkingMode() == WM_IntraMode) {
				
				IntraModeContext *intraCtx;
				ret->setCurrentFunction((ADDRINT)-1);
				intraCtx = ret->getCurrentFunctionCtx();

				t=b=0;
				traceObject(globalSharedContext->allBlocks[(ADDRINT)-1], intraCtx, t, b);
				intraCtx->shadowStack.push(ShadowStackItemType(t,b,(ADDRINT)-1));
			}

		} else {

			traceObject(globalSharedContext->allBlocks[(ADDRINT)-1], ret, ret->treeTop, ret->treeBottom);
		}
		
	}

	return ret;
}

GlobalContext::GlobalContext(WorkingModeType wm, unsigned kval, optionsClass options) : 
	_K_CCF_VAL(kval), _WorkingMode(wm)
{
	startFuncAddr=0;
	stopFuncAddr=0;
	exitPassed=false;

	threadContexts.reserve(64);
	InitLock(&lock);

	this->options = options;

#if DEBUG
	showDebug=true;
#endif

}

GlobalContext::~GlobalContext() {

	for (unsigned i=0; i < threadContexts.size(); i++)
		delete threadContexts[i];

	for (BlocksMapIt it = allBlocks.begin(); it != allBlocks.end(); it++)
		delete it->second;

	for (FuncsMapIt it = allFuncs.begin(); it != allFuncs.end(); it++)
		delete it->second;
}


