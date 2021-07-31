
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

void GlobalContext::setupThreadContext(ThreadContext *threadCtx)
{
    WorkingModeType wMode = globalSharedContext->WorkingMode();

    if (globalSharedContext->funcsToTrace.empty())
        return;

    if (wMode == WM_InterProcMode) {

        traceObject(
            globalSharedContext->allBlocks[(ADDRINT)-1],
            threadCtx,
            threadCtx->treeTop,
            threadCtx->treeBottom
        );

        return;
    }

    // wMode is WM_FuncMode or WM_IntraMode.

    ihppNode *t=0,*b=0;
    traceObject(globalSharedContext->allFuncs[(ADDRINT)-1], threadCtx, t, b);

#if ENABLE_KEEP_STACK_PTR
    threadCtx->shadowStack.push(ShadowStackItemType(t,b,(ADDRINT)-1));
#else
    threadCtx->shadowStack.push(ShadowStackItemType(t,b));
#endif

    if (wMode == WM_IntraMode) {

        IntraModeContext *intraCtx;
        threadCtx->setCurrentFunction((ADDRINT)-1);
        intraCtx = threadCtx->getCurrentFunctionCtx();

        t=b=0;
        traceObject(globalSharedContext->allBlocks[(ADDRINT)-1], intraCtx, t, b);

#if ENABLE_KEEP_STACK_PTR
        intraCtx->shadowStack.push(ShadowStackItemType(t,b,(ADDRINT)-1));
#else
        intraCtx->shadowStack.push(ShadowStackItemType(t,b));
#endif
    }
}

ThreadContext *GlobalContext::getThreadCtx(PIN_THREAD_UID tid) {

    ThreadContext *ret;
    PIN_GetLock(&lock, 1);

    if (!threadContexts.size()) {

        double tmp = getMilliseconds();
        double diff = tmp - timer;
        timer = tmp;

        fprintf(stderr, "[ IHPP ] Instrumentation time: %.3f sec\n", diff/1000.0);

#if EMPTY_ANALYSIS
        threadContexts.push_back(new ThreadContext(0, 0, 0));
#endif
    }


#if EMPTY_ANALYSIS
    PIN_ReleaseLock(&lock);
    return 0;
#endif

    for (unsigned i=0; i < threadContexts.size(); i++) {

        if (threadContexts[i]->getThreadID() == tid) {

            ret = threadContexts[i];
            PIN_ReleaseLock(&lock);

            return ret;
        }
    }

    ret = new ThreadContext(tid, startFuncAddr, stopFuncAddr);
    threadContexts.push_back(ret);
    PIN_ReleaseLock(&lock);
    setupThreadContext(ret);
    return ret;
}

GlobalContext::GlobalContext(WorkingModeType wm, unsigned kval, optionsClass options)
	: _K_CCF_VAL(kval)
	, _WorkingMode(wm)
{
    startFuncAddr=0;
    stopFuncAddr=0;
    exitPassed=false;

    threadContexts.reserve(64);
    PIN_InitLock(&lock);

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


