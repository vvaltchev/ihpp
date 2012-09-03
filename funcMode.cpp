
#include "config.h"
#include "debug.h"
#include "ihpp.h"

using namespace std;

#include "benchmark.h"
#include "output.h"

#include "tracingFuncs.h"

#define FUNCMODE_TOP_STACKPTR()				(ctx->shadowStack.size()?ctx->shadowStack.top().stackPtr:(ADDRINT)-1)

#if ENABLE_RELY_ON_SP_CHECK
inline void funcMode_sp_check(ThreadContext *ctx, ADDRINT reg_sp)
{

#if ENABLE_INS_TRACING
	if (globalSharedContext->options.insTracing)
		return;
#endif
	
	if (reg_sp >= FUNCMODE_TOP_STACKPTR()) {

		dbg_functr_regsp_gt();

		while (ctx->shadowStack.size() > 1 && reg_sp >= FUNCMODE_TOP_STACKPTR()) {

			dbg_functr_pop();

			if (!ctx->popShadowStack())
				break;
		}

	}
}
#endif


void FunctionObjTrace(FunctionObj *fc

#if ENABLE_KEEP_STACK_PTR
	, ADDRINT reg_sp
#endif
) {

	ihppNode *treeTop=0;
	ihppNode *treeBottom=0;	
	ThreadContext *ctx;
	GlobalContext *globalCtx = globalSharedContext;


#if !ENABLE_KEEP_STACK_PTR
	const ADDRINT reg_sp = (ADDRINT)-1;
#endif

	ctx = globalCtx->getThreadCtx(PIN_ThreadUid());

#if EMPTY_ANALYSIS
	return;
#endif

	if (fc->functionAddress() == ctx->startFuncAddr)
		ctx->haveToTrace=true;

	if (!ctx->haveToTrace)
		return;

	if (fc->functionAddress() == ctx->stopFuncAddr)
		ctx->haveToTrace=false;

	ctx->setCurrentFunction(fc->functionAddress());

	fc->incSimpleCounter();


	if (!globalCtx->exitPassed && fc->functionAddress() == globalSharedContext->spAttrs.exit_addr)
		globalCtx->exitPassed=true;



#if defined(_WIN32) && ENABLE_WIN32_MAIN_ALIGNMENT

	win32_main_alignment(ctx, fc);

#endif

	dbg_functr_funcname();
	dbg_functr_stackptr();

#if ENABLE_RELY_ON_SP_CHECK

	funcMode_sp_check(ctx, reg_sp);

#endif


	if (ctx->shadowStack.size()) {

		treeTop = ctx->shadowStack.top().treeTop; 
		treeBottom = ctx->shadowStack.top().treeBottom;
	} 

	assert(ctx->shadowStack.size() || !ctx->rootKey);

	if (globalCtx->options.showCalls)
		funcTraceDebugDump(globalCtx, fc, ctx, reg_sp, treeTop, treeBottom);

	traceObject(fc, ctx, treeTop, treeBottom);

#if ENABLE_KEEP_STACK_PTR
	ctx->shadowStack.push(ShadowStackItemType(treeTop,treeBottom,reg_sp));
#else
	ctx->shadowStack.push(ShadowStackItemType(treeTop,treeBottom));
#endif

#if ENABLE_INS_FORWARD_JMP_RECOGNITION

	if (ctx->forwardJmpHappened && ctx->fjmpsFuncAddr == fc->functionAddress()) {

		ctx->forwardJmpHappened=false;
		ctx->shadowStack.top().fjmps = ctx->lastfjmps+1;
		dbg_functr_fjmps_set();
	}

#endif

	dbg_functr_ssize_after();
}


void funcMode_ret()
{
	GlobalContext *globalCtx = globalSharedContext;
	ThreadContext *ctx;
	bool fjmps_happened=false;

#if EMPTY_ANALYSIS
	return;
#endif

	ctx = globalCtx->getThreadCtx(PIN_ThreadUid());

	if (!ctx->haveToTrace)
		return;

	dbg_funcret_name();

	assert( globalCtx->funcsToTrace.size() || (ctx->shadowStack.size() || globalCtx->exitPassed) );

#if ENABLE_INS_FORWARD_JMP_RECOGNITION

	dbg_funcret_fjmps();

	if (ctx->shadowStack.top().fjmps)
		fjmps_happened=true;

	while (ctx->shadowStack.top().fjmps--) 
	{
		if (ctx->shadowStack.size() <= 1)
			break;

		if (!ctx->canPopStack())
			break;

		dbg_functr_pop();
		ctx->shadowStack.pop();
		ctx->counter = ctx->counter ? ctx->counter-1 : 0;
		
		if (globalCtx->WorkingMode() == WM_IntraMode)
			intraMode_ret();
	}

#endif

	if (ctx->shadowStack.size() == 1) {

		dbg_funcret_pop_err();
		goto function_end;
	}

	dbg_funcret_pop();

	if (ctx->canPopStack() && !fjmps_happened) {
		
		dbg_functr_pop();
		ctx->shadowStack.pop();
		ctx->counter = ctx->counter ? ctx->counter-1 : 0;
	}

	dbg_funcret_stack_after_pop();

function_end:

	if (globalCtx->WorkingMode() == WM_IntraMode)
		intraMode_ret();

	if (ctx->shadowStack.size())
		ctx->setCurrentFunction(ctx->shadowStack.top().treeTop->getValue()->getKey());
}
