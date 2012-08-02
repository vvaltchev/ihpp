
#include "config.h"
#include "debug.h"
#include "ihpp.h"

using namespace std;

#include "benchmark.h"
#include "output.h"

#include "tracingFuncs.h"

#define FUNCMODE_TOP_STACKPTR()				(ctx->shadowStack.size()?ctx->shadowStack.top().stackPtr:(ADDRINT)-1)
#define FUNCMODE_LOAD_TOP_BOTTOM()			treeTop = ctx->shadowStack.top().treeTop; treeBottom = ctx->shadowStack.top().treeBottom;
#define FUNCMODE_STORE_TOP_BOTTOM(sp)		ctx->shadowStack.push(ShadowStackType(treeTop,treeBottom,(sp)));
#define FUNCMODE_SET_TOP_BOTTOM_TO_ROOT()	ctx->counter=0; treeTop=ctx->kSlabForest.getTreeRef(ctx->rootKey); treeBottom=0;

inline void funcMode_sp_check(ihppThreadContextClass *ctx, ADDRINT reg_sp)
{
	if (reg_sp >= FUNCMODE_TOP_STACKPTR()) {
	
		dbg_functr_regsp_gt();

		while (ctx->shadowStack.size() > 1 && reg_sp >= FUNCMODE_TOP_STACKPTR()) {
		
			dbg_functr_pop();
			
			if (!ctx->popShadowStack())
				break;
		}

	}
}


VOID FunctionObjTrace(FunctionObj *fc, ADDRINT reg_sp) {

	ihppNode *treeTop=0;
	ihppNode *treeBottom=0;	
	ihppThreadContextClass *ctx;
	ihppContextClass *globalCtx = globalSharedContext;

	ctx = globalCtx->getThreadCtx(PIN_ThreadUid());
	
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
		
		FUNCMODE_LOAD_TOP_BOTTOM();
	
	} 

	//assert(ctx->shadowStack.size() || !ctx->rootKey);

	//stackSize == 0 && rootKey != null
	//This should never happen for functions in full trace mode
	//because stack size is FORCED to be > 0 after the first function call
	//but it can happen in funcMode or intraMode when tracing only a few functions,
	//or very often when tracing only one function.

	else if (ctx->rootKey) {

		//Reset top,bottom pointers to root of the function's tree
		FUNCMODE_SET_TOP_BOTTOM_TO_ROOT();

		if (globalCtx->options.showCalls)
			funcTraceDebugDump(globalCtx, fc, ctx, reg_sp, treeTop, treeBottom);

		//Push them on the shadow stack a increment the root counter
		FUNCMODE_STORE_TOP_BOTTOM((ADDRINT)-1);
		treeTop->incCounter();

		goto before_ret;
	}

	/////////////////////////////////////////////
	
	if (globalCtx->options.showCalls)
		funcTraceDebugDump(globalCtx, fc, ctx, reg_sp, treeTop, treeBottom);

	traceObject(fc, ctx, treeTop, treeBottom);
	
	FUNCMODE_STORE_TOP_BOTTOM(reg_sp);


before_ret:

#if ENABLE_INS_FORWARD_JMP_RECOGNITION
	
	if (ctx->forwardJmpHappened && ctx->fjmpsFuncAddr == fc->functionAddress()) {
	
		ctx->forwardJmpHappened=false;
		ctx->shadowStack.top().fjmps = ctx->lastfjmps+1;
		dbg_functr_fjmps_set();
	}

#endif

	return;
}


void funcMode_ret()
{
	ihppContextClass *globalCtx = globalSharedContext;
	ihppThreadContextClass *ctx;

	ctx = globalCtx->getThreadCtx(PIN_ThreadUid());

	if (!ctx->haveToTrace)
		return;

	dbg_funcret_name();
		
	assert( ctx->shadowStack.size() || globalCtx->exitPassed );

#if ENABLE_INS_FORWARD_JMP_RECOGNITION
	
	dbg_funcret_fjmps();

	while (ctx->shadowStack.top().fjmps--) 
	{
		if (ctx->shadowStack.size() <= 1)
			break;

		if (ctx->canPopStack())
			ctx->shadowStack.pop();
		else
			break;

		if (globalCtx->WorkingMode() == WM_IntraMode)
			intraMode_ret();
	}

#endif

	if (ctx->shadowStack.size() == 1) {
		
		dbg_funcret_pop_err();
		goto function_end;
	}
	
	dbg_funcret_pop();

	if (ctx->canPopStack())
		ctx->shadowStack.pop();

	dbg_funcret_stack_after_pop();

	dbg_funcret_new_call();


function_end:

	if (globalCtx->WorkingMode() == WM_IntraMode)
		intraMode_ret();

	ctx->setCurrentFunction(ctx->shadowStack.top().treeTop->getValue()->getKey());
}
