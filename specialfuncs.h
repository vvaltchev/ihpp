
#pragma once
#include "debug.h"


#if defined(_WIN32) && ENABLE_WIN32_MAIN_ALIGNMENT
    #define INIT_THREAD_CTX_W32_VARS()            __tmainCRTStartup_stack_size=-1;
#else
    #define INIT_THREAD_CTX_W32_VARS()
#endif


#ifdef _WIN32
    #define CHECK_LONGJMP_NOTIFY()                (win32_check_nlg_notify(ctx, currFuncAddr))
#else
    #define CHECK_LONGJMP_NOTIFY()                0
#endif


#if ENABLE_SUBCALL_CHECK

    #define SUBCALL_COUNT_INC()                    if (ctx->subcallcount < (UINT32)-1) ctx->subcallcount++;

    #define SUBCALL_RESET()                        ctx->subcallcount=0;

    #define SUBCALL_FALSE_JMP_CHECK()              (subcall_false_jmp_check(ctx,currFuncAddr))

    #define SUBCALL_MAIN_CHECK()                  \
        if (insCat == XED_CATEGORY_CALL &&        \
            currentFuncAddr == targetFuncAddr &&  \
            targetFuncAddr != targetAddr)         \
        {                                         \
            dbg_brcall_subcall();                 \
            ctx->subcallcount=0;                  \
        }

    #define INIT_SUBCALL_CHECK_VARS()            subcallcount=0;

#else

    #define SUBCALL_COUNT_INC()
    #define SUBCALL_RESET()
    #define SUBCALL_FALSE_JMP_CHECK()            0
    #define SUBCALL_MAIN_CHECK()
    #define INIT_SUBCALL_CHECK_VARS()

#endif

#ifdef _WIN32

    #define IS_WIN32_NLG_NOTIFY(func)                                         \
        ((func) == globalSharedContext->spAttrs._NLG_Notify_addr ||           \
        (func) == globalSharedContext->spAttrs._NLG_Notify1_addr ||           \
        (func) == globalSharedContext->spAttrs.__NLG_Dispatch_addr)

    #define FUNC_IS_TEXT(func)                                                \
        ((func) == globalSharedContext->spAttrs.text_addr ||                  \
        (func) == globalSharedContext->spAttrs.unnamedImageEntryPoint_addr)

    #define FUNC_IS_TEXT_N(func)                                              \
        ((func) == ".text" || (func) == "unnamedImageEntryPoint")

#else

    #define IS_WIN32_NLG_NOTIFY(func)           0
    #define FUNC_IS_TEXT(func)                  ((func) == globalSharedContext->spAttrs.text_addr)
    #define FUNC_IS_TEXT_N(func)                ((func) == ".text")

#endif
