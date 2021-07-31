
#include "config.h"
#include "debug.h"
#include "ihpp.h"

using namespace std;

#include "benchmark.h"
#include "output.h"

#include "tracingFuncs.h"

const string& ThreadContext::getCurrentFunctionName() const {
    return globalSharedContext->allFuncs[currentFunction]->functionName();
}

void GlobalContext::destroyThreadContext(PIN_THREAD_UID tid)
{
    // We have just reset the TLS for this thread ID and in *no case*
    // delete the threadContext object, as we'll need it later in output.cpp.
    PIN_SetThreadData(_tls_key, nullptr, tid);
}

void GlobalContext::createThreadContext(PIN_THREAD_UID tid)
{
    if (EMPTY_ANALYSIS) {
        if (threadContexts.size() > 0)
            return;
    }

    ThreadContext *threadCtx = new ThreadContext(tid, startFuncAddr, stopFuncAddr);
    PIN_SetThreadData(_tls_key, threadCtx, tid);
    threadContexts.push_back(threadCtx);

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

ThreadContext *GlobalContext::getThreadCtx(PIN_THREAD_UID tid)
{
    static bool already_called;

    if (!already_called) {
        double tmp = getMilliseconds();
        double diff = tmp - timer;
        fprintf(stderr, "[ IHPP ] Instrumentation time: %.3f sec\n", diff/1000.0);
        timer = tmp;
        already_called = true;
    }

    if (EMPTY_ANALYSIS)
        return nullptr;

    return (ThreadContext *)PIN_GetThreadData(_tls_key, tid);
}

GlobalContext::GlobalContext(WorkingModeType wm, unsigned kval, optionsClass options)
	: _K_CCF_VAL(kval)
	, _WorkingMode(wm)
    , exitPassed(false)
#if DEBUG
    , showDebug(true)
#endif
    , options(options)
    , startFuncAddr(0)
    , stopFuncAddr(0)
{
    _tls_key = PIN_CreateThreadDataKey(nullptr);

    if (_tls_key == INVALID_TLS_KEY) {
        cerr << "Unable to create TLS KEY" << endl;
        PIN_ExitProcess(1);
    }
}

GlobalContext::~GlobalContext() {

    PIN_DeleteThreadDataKey(_tls_key);

    for (BlocksMapIt it = allBlocks.begin(); it != allBlocks.end(); it++)
        delete it->second;

    for (FuncsMapIt it = allFuncs.begin(); it != allFuncs.end(); it++)
        delete it->second;
}


