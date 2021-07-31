
#include "specialfuncs.h"
#include "dataStructures.h"
#include "intraModeContext.h"

#ifndef __THREAD_CTX_HEADER__
#define __THREAD_CTX_HEADER__

void funcMode_ret();


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
    map<ADDRINT, IntraModeContext*> intraModeContexts;

    //Methods

    ThreadContext(PIN_THREAD_UID tid, ADDRINT startFuncAddr, ADDRINT stopFuncAddr);

    ~ThreadContext();

    inline bool canPopStack();
    inline bool popShadowStack();

    IntraModeContext *getFunctionCtx(ADDRINT funcAddr);
    IntraModeContext *getCurrentFunctionCtx();

    ADDRINT getCurrentFunction();
    IntraModeContext *setCurrentFunction(ADDRINT currFunc);
    string getCurrentFunctionName();

    PIN_THREAD_UID getThreadID() { return threadID; }
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

inline ThreadContext::ThreadContext(PIN_THREAD_UID tid, ADDRINT startFuncAddr, ADDRINT stopFuncAddr)
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

inline ADDRINT ThreadContext::getCurrentFunction()
{
    return currentFunction;
}

inline ThreadContext::~ThreadContext()
{
    for (map<ADDRINT, IntraModeContext*>::iterator it = intraModeContexts.begin(); it != intraModeContexts.end(); it++)
        delete it->second;
}


inline IntraModeContext *ThreadContext::getCurrentFunctionCtx()
{
    return getFunctionCtx(currentFunction);
}


inline IntraModeContext *ThreadContext::setCurrentFunction(ADDRINT currFunc)
{
    currentFunction=currFunc;
    return getCurrentFunctionCtx();
}


inline IntraModeContext *ThreadContext::getFunctionCtx(ADDRINT funcAddr)
{

    if (intraModeContexts.find(funcAddr) == intraModeContexts.end()) {

        IntraModeContext *intraCtx;
        intraCtx = new IntraModeContext(funcAddr);
        intraModeContexts[funcAddr] = intraCtx;
        return intraCtx;
    }

    return intraModeContexts[funcAddr];
}

#endif
