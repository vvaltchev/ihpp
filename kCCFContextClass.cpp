
#include "config.h"
#include "debug.h"
#include "kCCF.h"

using namespace std;
using namespace kCCFLib;

extern kCCFContextClass *globalSharedContext;

#include "benchmark.h"
#include "output.h"

#include "tracingFuncs.h"

namespace kCCFLib {

	
kCCFThreadContextClass *kCCFContextClass::getThreadCtx(PIN_THREAD_UID tid) { 
	
	kCCFThreadContextClass *ret;

	GetLock(&lock, 1);

	for (unsigned i=0; i < threadContexts.size(); i++) {
		
		if (threadContexts[i]->threadID == tid) {
			
			ret = threadContexts[i];
			ReleaseLock(&lock);

			return ret;
		}
	}

	threadContexts.push_back(new kCCFThreadContextClass(tid, startFuncAddr, stopFuncAddr));

	ret = threadContexts.back();
	ReleaseLock(&lock);

	return ret;
}

kCCFContextClass::kCCFContextClass(unsigned int k, WorkingModeType wm) : 
	_K_CCF_VAL(k), _WorkingMode(wm)
{
	startFuncAddr=0;
	stopFuncAddr=0;
	exitPassed=false;

	threadContexts.reserve(64);
	InitLock(&lock);
}

kCCFContextClass::~kCCFContextClass() {

	for (unsigned i=0; i < threadContexts.size(); i++)
		delete threadContexts[i];

	for (BlocksMapIt it = allBlocks.begin(); it != allBlocks.end(); it++)
		delete it->second;

	for (FuncsMapIt it = allFuncs.begin(); it != allFuncs.end(); it++)
		delete it->second;
}



}