
#ifndef __SPECIAL_FUNCS_MACROS__
#define __SPECIAL_FUNCS_MACROS__



#if defined(_WIN32) && ENABLE_WIN32_MAIN_ALIGNMENT
	//#define THREAD_CTX_POP_CHECK()				(__tmainCRTStartup_stack_size == (unsigned)-1 || shadowStack.size()-1 >= __tmainCRTStartup_stack_size+1)
	#define INIT_THREAD_CTX_W32_VARS()			__tmainCRTStartup_stack_size=-1;

#else
	//#define THREAD_CTX_POP_CHECK()				(shadowStack.size())
	#define INIT_THREAD_CTX_W32_VARS()
#endif



#ifdef _WIN32
	#define CHECK_LONGJMP_NOTIFY()				(win32_check_nlg_notify(ctx, currFuncAddr))
	#define FUNC_IS_TEXT(func)					((func) == ".text" || (func) == "unnamedImageEntryPoint")
#else
	#define CHECK_LONGJMP_NOTIFY()				0
	#define FUNC_IS_TEXT(func)					((func) == ".text")
#endif



#if ENABLE_SUBCALL_CHECK

	#define SUBCALL_COUNT_INC()					if (ctx->subcallcount < (UINT32)-1) ctx->subcallcount++;

	#define SUBCALL_RESET()						ctx->subcallcount=0;

	#define SUBCALL_FALSE_JMP_CHECK()			(subcall_false_jmp_check(ctx,currFuncAddr))

	#define SUBCALL_MAIN_CHECK()				if (insCat == XED_CATEGORY_CALL && currentFuncAddr == targetFuncAddr && targetFuncAddr != targetAddr) \
												{																									  \
													dbg_brcall_subcall();																			  \
													ctx->subcallcount=0;																			  \
												}

	#define INIT_SUBCALL_CHECK_VARS()			subcallcount=0; 

#else

	#define SUBCALL_COUNT_INC()
	#define SUBCALL_RESET()
	#define SUBCALL_FALSE_JMP_CHECK()			0
	#define SUBCALL_MAIN_CHECK()
	#define INIT_SUBCALL_CHECK_VARS()

#endif

#endif


#ifdef __THREAD_CTX_HEADER__

namespace kCCFLib {


#ifdef _WIN32
	
#define IS_WIN32_NLG_NOTIFY(func)			((func) == globalSharedContext->spAttrs._NLG_Notify_addr ||  \
											 (func) == globalSharedContext->spAttrs._NLG_Notify1_addr || \
											 (func) == globalSharedContext->spAttrs.__NLG_Dispatch_addr)
#else
	#define IS_WIN32_NLG_NOTIFY(func)			0

#endif

#ifdef _WIN32

#if ENABLE_WIN32_MAIN_ALIGNMENT
inline void win32_main_alignment(kCCFThreadContextClass *ctx, FunctionObj *fc)
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
inline bool win32_check_nlg_notify(kCCFThreadContextClass *ctx, ADDRINT currFuncAddr)
{
	
	if (IS_WIN32_NLG_NOTIFY(currFuncAddr)) {
		ctx->jumpTargetFuncAddr=currFuncAddr;
		ctx->haveToJump=true;
		return true;
	}

	return false;
}
#endif

#endif

#if ENABLE_SUBCALL_CHECK && ENABLE_INS_TRACING

inline bool subcall_false_jmp_check(kCCFThreadContextClass *ctx, ADDRINT currFuncAddr)
{
	if (ctx->subcallcount <= 2)						
	{												
		dbg_single_inst_false_jmp();				
		ctx->subcallcount=0;						
		ctx->jumpTargetFuncAddr = currFuncAddr;		
		return true;										
	}

	return false;
}

#endif

}

#endif

