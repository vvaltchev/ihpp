
#include "config.h"
#include "debug.h"
#include "ihpp.h"

using namespace std;

#include "benchmark.h"
#include "output.h"

#include "tracingFuncs.h"


	
string ihppThreadContextClass::getCurrentFunctionName() { 
	return globalSharedContext->allFuncs[currentFunction]->functionName(); 
}

ihppThreadContextClass *ihppContextClass::getThreadCtx(PIN_THREAD_UID tid) { 
	
	ihppThreadContextClass *ret;

	GetLock(&lock, 1);

	for (unsigned i=0; i < threadContexts.size(); i++) {
		
		if (threadContexts[i]->threadID == tid) {
			
			ret = threadContexts[i];
			ReleaseLock(&lock);

			return ret;
		}
	}

	threadContexts.push_back(new ihppThreadContextClass(tid, startFuncAddr, stopFuncAddr));

	ret = threadContexts.back();
	ReleaseLock(&lock);

	return ret;
}

ihppContextClass::ihppContextClass(WorkingModeType wm, unsigned kval, optionsClass options) : 
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

ihppContextClass::~ihppContextClass() {

	for (unsigned i=0; i < threadContexts.size(); i++)
		delete threadContexts[i];

	for (BlocksMapIt it = allBlocks.begin(); it != allBlocks.end(); it++)
		delete it->second;

	for (FuncsMapIt it = allFuncs.begin(); it != allFuncs.end(); it++)
		delete it->second;
}

