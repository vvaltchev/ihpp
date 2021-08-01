#pragma once

#include "specialfuncs.h"
#include "dataStructures.h"
#include "intraModeContext.h"
#include "tracingFuncs.h"

class ThreadContext : public GenericTraceContext {

    ADDRINT currentFunction;
    PIN_THREAD_UID threadID;

public:

    //WM_InterProcMode properties

    ihppNode *treeTop;
    ihppNode *treeBottom;

    //WM_FuncMode properties

    ADDRINT startFuncAddr;
    ADDRINT stopFuncAddr;
    bool haveToTrace;

#if ENABLE_INS_TRACING
    ADDRINT jumpTargetFuncAddr;
    ADDRINT lastJumpTargetFuncAddr;
    bool haveToJump;
#endif

#if ENABLE_SUBCALL_CHECK
    UINT32 subcallcount;
#endif


#if defined(_WIN32) && ENABLE_WIN32_MAIN_ALIGNMENT
    size_t __tmainCRTStartup_stack_size;
#endif

#if ENABLE_INS_FORWARD_JMP_RECOGNITION
    bool forwardJmpHappened;
    unsigned int lastfjmps;
    ADDRINT fjmpsFuncAddr;
#endif

    //WM_IntraMode properties
    std::map<ADDRINT, IntraModeContext*> intraModeContexts;

    //Methods

    ThreadContext(PIN_THREAD_UID tid, ADDRINT startFuncAddr, ADDRINT stopFuncAddr);
    ~ThreadContext();

    bool canPopStack();
    bool popShadowStack();

    IntraModeContext *setCurrentFunction(ADDRINT currFunc);
    IntraModeContext *getFunctionCtx(ADDRINT funcAddr);
    IntraModeContext *getCurrentFunctionCtx() { return getFunctionCtx(currentFunction); }

    ADDRINT getCurrentFunction() const { return currentFunction; }
    const std::string& getCurrentFunctionName() const;
    PIN_THREAD_UID getThreadID() const { return threadID; }
};

inline bool ThreadContext::canPopStack() {

#if defined(_WIN32) && ENABLE_WIN32_MAIN_ALIGNMENT

    return shadowStack.size() > 0 &&
            (__tmainCRTStartup_stack_size == (unsigned)-1 ||
            shadowStack.size()-1 >= __tmainCRTStartup_stack_size+1);

#else

    return shadowStack.size() > 0;

#endif
}

inline bool ThreadContext::popShadowStack()
{
    if (canPopStack()) {

        funcMode_ret();
        return true;
    }

    return false;
}

inline
ThreadContext::ThreadContext(PIN_THREAD_UID tid, ADDRINT startFuncAddr, ADDRINT stopFuncAddr)
    :  GenericTraceContext()
    ,  currentFunction(0)
    , threadID(tid)
    , treeTop(nullptr)
    , treeBottom(nullptr)
    , startFuncAddr(startFuncAddr)
    , stopFuncAddr(stopFuncAddr)
    , haveToTrace(!startFuncAddr)
{
    INIT_SUBCALL_CHECK_VARS();
    INIT_THREAD_CTX_W32_VARS();

#if ENABLE_INS_TRACING
    jumpTargetFuncAddr=0;
    lastJumpTargetFuncAddr=0;
    haveToJump=false;
#endif

#if ENABLE_INS_FORWARD_JMP_RECOGNITION
    forwardJmpHappened=false;
    lastfjmps=0;
    fjmpsFuncAddr=0;
#endif
}

inline ThreadContext::~ThreadContext()
{
    for (const auto& it : intraModeContexts)
        delete it.second;
}

inline IntraModeContext *ThreadContext::setCurrentFunction(ADDRINT currFunc)
{
    currentFunction = currFunc;
    return getCurrentFunctionCtx();
}

inline IntraModeContext *ThreadContext::getFunctionCtx(ADDRINT funcAddr)
{
    const auto& it = intraModeContexts.find(funcAddr);

    if (it == intraModeContexts.end()) {

        IntraModeContext *intraCtx = new IntraModeContext(funcAddr);
        intraModeContexts[funcAddr] = intraCtx;
        return intraCtx;
    }

    return it->second;
}
