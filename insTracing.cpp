
#include "config.h"
#include "debug.h"
#include "ihpp.h"

using namespace std;

#include "tracingFuncs.h"

#if ENABLE_INS_TRACING

static void getTargetFunc(ADDRINT& targetAddr, ADDRINT& targetFuncAddr)
{
    RTN rtn;
    PIN_LockClient();
    {
        rtn = RTN_FindByAddress(targetAddr);

        if(RTN_Valid(rtn))
            targetFuncAddr = RTN_Address(rtn);
    }
    PIN_UnlockClient();
}

void PIN_FAST_ANALYSIS_CALL singleInstruction(ADDRINT currFuncAddr)
{
    GlobalContext *globalCtx = globalSharedContext;
    ThreadContext *ctx;

    ctx = globalCtx->getThreadCtx(PIN_ThreadUid());

#if EMPTY_ANALYSIS
    return;
#endif

    if (currFuncAddr == ctx->startFuncAddr)
        ctx->haveToTrace=true;

    if (!ctx->haveToTrace)
        return;

    if (currFuncAddr == ctx->stopFuncAddr)
        ctx->haveToTrace=false;

    if (CHECK_LONGJMP_NOTIFY())
        return;

    SUBCALL_COUNT_INC();

    if (ctx->haveToJump) {

        dbg_single_inst_have_to_jmp();

        ctx->popShadowStack();

        SUBCALL_RESET();

        ctx->haveToJump=false;
        ctx->jumpTargetFuncAddr=currFuncAddr;
        return;
    }

    /*
        The below method of longjmp discovering is disabled when tracing is enabled for all functions
        due to TOO much false positives (under win32): false longjmps discovering through subcall_check
        isn't always enought.
    */

#if defined(_WIN32) && !ENABLE_WIN32_FULLTRACE_TARGET_TRACING_LONGJMP

    if (!globalCtx->funcsToTrace.size()) {
        ctx->jumpTargetFuncAddr=currFuncAddr;
        return;
    }

#endif

    /*
        This don't work when: a() and c() are traced, and b() is NOT traced.
        If a() calls b() and b() calls c():
        a()
            b()
                c()
        Since b() is NOT traced, the situation APPEARS like that:
        a()
        c() [after longjmp]


    */
    if (ctx->jumpTargetFuncAddr && currFuncAddr != ctx->jumpTargetFuncAddr) {

        if (SUBCALL_FALSE_JMP_CHECK())
            return;

        //Long jump happened

        dbg_single_inst_reg_jmp();

        assert(ctx->shadowStack.size());

        ctx->popShadowStack();

        SUBCALL_RESET();

        ctx->jumpTargetFuncAddr=currFuncAddr;
    }
}

void indirect_branchOrCall(ADDRINT currentFuncAddr,
                           ADDRINT targetAddr,
                           ADDRINT insCat)
{
    ADDRINT targetFuncAddr=0;

#if EMPTY_ANALYSIS
    return;
#endif

    getTargetFunc(targetAddr, targetFuncAddr);
    branchOrCall(currentFuncAddr, targetAddr, targetFuncAddr, insCat);
}

void branchOrCall(ADDRINT currentFuncAddr,
                  ADDRINT targetAddr,
                  ADDRINT targetFuncAddr,
                  ADDRINT insCat)
{
    bool traceTarget;
    ThreadContext *ctx;

#if EMPTY_ANALYSIS
    return;
#endif

#if DEBUG

    string currentFuncName;
    string targetFuncName;
    RTN rtn;

    PIN_LockClient();
    {
        rtn = RTN_FindByAddress(currentFuncAddr);

        if(RTN_Valid(rtn))
            currentFuncName = RTN_Name(rtn);

        rtn = RTN_FindByAddress(targetFuncAddr);

        if (RTN_Valid(rtn))
            targetFuncName = RTN_Name(rtn);
    }
    PIN_UnlockClient();

#endif

    ctx = globalSharedContext->getThreadCtx(PIN_ThreadUid());

    if (currentFuncAddr == ctx->startFuncAddr)
        ctx->haveToTrace=true;

    if (!ctx->haveToTrace)
        return;

    if (currentFuncAddr == ctx->stopFuncAddr)
        ctx->haveToTrace=false;

    if (!targetFuncAddr)
        return;

    traceTarget = globalSharedContext->hasToTrace(targetFuncAddr);

    /*
        This often produces false positives: under win32 when a JMP with a target which is outside current function is met,
        this doesn't mean that that's is a long jmp: sometimes JMP is used as a light function-call convention..
    */
    if (insCat == XED_CATEGORY_UNCOND_BR)
        if (traceTarget && targetFuncAddr != currentFuncAddr && !FUNC_IS_TEXT(currentFuncAddr)) {

            dbg_brcall_jmp();

            if (ctx->shadowStack.size() > 1) {

#if ENABLE_INS_FORWARD_JMP_RECOGNITION

                bool found=false;

                for (size_t i=1; i < ctx->shadowStack.size(); i++) {

                    ADDRINT beforeLastTop = ctx->shadowStack.top(i).treeTop->getKey();

                    if (targetFuncAddr == beforeLastTop) {

                        found=true;
                        ctx->popShadowStack();
                        break;
                    }

                }

                if (!found) {

                    dbg_brcall_fjmp_found1();
                    ctx->forwardJmpHappened=true;
                    ctx->fjmpsFuncAddr=targetFuncAddr;
                    ctx->lastfjmps = ctx->shadowStack.top().fjmps;
                }
#else

                ctx->popShadowStack();
#endif


            } else {

#if ENABLE_INS_FORWARD_JMP_RECOGNITION

                dbg_brcall_fjmp_found2();
                ctx->forwardJmpHappened=true;
                ctx->fjmpsFuncAddr=targetFuncAddr;
                ctx->lastfjmps = ctx->shadowStack.top().fjmps;
#endif
                dbg_brcall_cantpop();
            }

        }


    dbg_brcall_ifret();

    if (!traceTarget && FUNC_IS_TEXT(currentFuncAddr)) {

        dbg_brcall_keepoldjumpaddr();
        ctx->jumpTargetFuncAddr = ctx->lastJumpTargetFuncAddr;
        return;
    }

    SUBCALL_MAIN_CHECK();
    singleInstruction(currentFuncAddr);

    if (traceTarget || FUNC_IS_TEXT(targetFuncAddr)) {

        dbg_brcall_tracejmp();

        ctx->lastJumpTargetFuncAddr = ctx->jumpTargetFuncAddr;
        ctx->jumpTargetFuncAddr = targetFuncAddr;
    }
}

#endif
