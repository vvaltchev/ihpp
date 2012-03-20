
#include "config.h"
#include "debug.h"
#include "kCCF.h"

using namespace std;
using namespace kCCFLib;

extern kCCFContextClass *globalSharedContext;

#include "tracingFuncs.h"

inline void getCurrentAndTargetFunc(ADDRINT insAddr, ADDRINT targetAddr, 
									ADDRINT &currentFuncAddr, ADDRINT &targetFuncAddr, 
									string &currentFuncName, string &targetFuncName)
{
	RTN rtn;
	PIN_LockClient();
	
	rtn = RTN_FindByAddress(insAddr);
	
	if(RTN_Valid(rtn)) {
		currentFuncName = RTN_Name(rtn);
		currentFuncAddr = RTN_Address(rtn);
	}

	rtn = RTN_FindByAddress(targetAddr);
	
	if(RTN_Valid(rtn)) {
		targetFuncName = RTN_Name(rtn);
		targetFuncAddr = RTN_Address(rtn);
	}


	PIN_UnlockClient();	
}

#if ENABLE_INS_TRACING

void PIN_FAST_ANALYSIS_CALL singleInstruction(kCCFContextClass *globalCtx, ADDRINT currFuncAddr, ADDRINT insCat) {

	
	kCCFThreadContextClass *ctx;
	ctx = globalCtx->getThreadCtx(PIN_ThreadUid());	

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



void branchOrCall(kCCFContextClass *globalCtx, ADDRINT insAddr, ADDRINT targetAddr, ADDRINT insCat) {


	bool traceTarget;

	ADDRINT currentFuncAddr=0;
	ADDRINT targetFuncAddr=0;

	string currentFuncName;
	string targetFuncName;
	
	kCCFThreadContextClass *ctx;
	
	ctx = globalCtx->getThreadCtx(PIN_ThreadUid());	

	getCurrentAndTargetFunc(insAddr, targetAddr, currentFuncAddr, targetFuncAddr, currentFuncName, targetFuncName);

	if (currentFuncAddr == ctx->startFuncAddr)
		ctx->haveToTrace=true;

	if (!ctx->haveToTrace)
		return;
	
	if (currentFuncAddr == ctx->stopFuncAddr)
		ctx->haveToTrace=false;

	if (!targetFuncAddr)
		return;

	traceTarget = globalCtx->hasToTrace(targetFuncName, targetFuncAddr);
	
	/*
		This often produces false positives: under win32 when a JMP with a target which is outside current function is met,
		this doesn't mean that that's is a long jmp: sometimes JMP is used as a light function-call convention..
	*/
	if (insCat == XED_CATEGORY_UNCOND_BR)
		if (traceTarget && targetFuncAddr != currentFuncAddr && !FUNC_IS_TEXT(currentFuncName)) {
	
			dbg_brcall_jmp();
			
			if (ctx->shadowStack.size() > 1) {
				
#if INS_JMP_OUTSIDE_FUNC_PARENTS_CHECK

				for (size_t i=1; i < ctx->shadowStack.size(); i++) {

					ADDRINT beforeLastTop = ctx->shadowStack.top(i).treeTop->getKey();
					
					if (targetFuncAddr == beforeLastTop) {
				
						ctx->popShadowStack();
						break;
					} 

				}
#else

				ctx->popShadowStack();
#endif
				
				
			} else {

				dbg_brcall_cantpop();
			}

		} 


	dbg_brcall_ifret();	

	if (!traceTarget && FUNC_IS_TEXT(currentFuncName)) {
	
		dbg_brcall_keepoldjumpaddr();

		ctx->jumpTargetFuncAddr = ctx->lastJumpTargetFuncAddr;
		return;
	}


	SUBCALL_MAIN_CHECK();

	singleInstruction(globalCtx, currentFuncAddr, insCat);


	if (traceTarget || FUNC_IS_TEXT(targetFuncName)) {
	
		dbg_brcall_tracejmp();

		ctx->lastJumpTargetFuncAddr = ctx->jumpTargetFuncAddr;
		ctx->jumpTargetFuncAddr = targetFuncAddr;
	}
}

#endif