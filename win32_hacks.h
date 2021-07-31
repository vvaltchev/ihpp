#include "config.h"
#include "debug.h"

#ifdef _WIN32

#if ENABLE_WIN32_MAIN_ALIGNMENT
static void win32_main_alignment(ThreadContext *ctx, FunctionObj *fc)
{
    if (fc->functionAddress() == globalSharedContext->spAttrs.__tmainCRTStartup_addr)
        ctx->__tmainCRTStartup_stack_size = ctx->shadowStack.size();

    if (ctx->__tmainCRTStartup_stack_size == -1)
        return;

    if (fc->functionAddress() == globalSharedContext->spAttrs.wWinMain_addr ||
        fc->functionAddress() == globalSharedContext->spAttrs.main_addr) {

        if (ctx->shadowStack.size() > ctx->__tmainCRTStartup_stack_size+1) {

            cerr << "WARNING: artifical main() alignment\n";
        }

        while (ctx->shadowStack.size() > ctx->__tmainCRTStartup_stack_size+1)
            ctx->popShadowStack();
    }
}
#endif

#if ENABLE_INS_TRACING
inline bool win32_check_nlg_notify(ThreadContext *ctx, ADDRINT currFuncAddr)
{

    if (IS_WIN32_NLG_NOTIFY(currFuncAddr)) {
        ctx->jumpTargetFuncAddr=currFuncAddr;
        ctx->haveToJump=true;
        return true;
    }

    return false;
}
#endif

#endif // #ifdef _WIN32
