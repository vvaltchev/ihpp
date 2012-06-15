
#include "config.h"
#include "debug.h"
#include "kCCF.h"

using namespace std;
using namespace kCCFLib;

//extern kCCFContextClass *globalSharedContext;

#include "benchmark.h"
#include "output.h"

#include "tracingFuncs.h"

#define FUNCMODE_TOP_STACKPTR()				(ctx->shadowStack.size()?ctx->shadowStack.top().stackPtr:(ADDRINT)-1)
#define FUNCMODE_LOAD_TOP_BOTTOM()			treeTop = ctx->shadowStack.top().treeTop; treeBottom = ctx->shadowStack.top().treeBottom;
#define FUNCMODE_STORE_TOP_BOTTOM(sp)		ctx->shadowStack.push(ShadowStackType(treeTop,treeBottom,(sp)));
#define FUNCMODE_SET_TOP_BOTTOM_TO_ROOT()	ctx->counter=0; treeTop=ctx->kSlabForest.getTreeRef(ctx->rootKey); treeBottom=0;

inline void funcMode_sp_check(kCCFThreadContextClass *ctx, ADDRINT reg_sp)
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

	kCCFNode *treeTop=0;
	kCCFNode *treeBottom=0;	
	kCCFThreadContextClass *ctx;
	kCCFContextClass *globalCtx = globalSharedContext;

	ctx = globalCtx->getThreadCtx(PIN_ThreadUid());
	
	if (fc->functionAddress() == ctx->startFuncAddr)
		ctx->haveToTrace=true;

	if (!ctx->haveToTrace)
		return;

	if (fc->functionAddress() == ctx->stopFuncAddr)
		ctx->haveToTrace=false;

	ctx->setCurrentFunction(fc->functionAddress());

	fc->incSimpleCounter();


	if (!globalCtx->exitPassed && fc->functionName() == "exit")
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
	
	} else if (ctx->rootKey) {

		//Reset top,bottom pointers to root of the function's tree
		FUNCMODE_SET_TOP_BOTTOM_TO_ROOT();

		funcTraceDebugDump(globalCtx, fc, ctx, reg_sp, treeTop, treeBottom);

		//Push them on the shadow stack a increment the root counter
		FUNCMODE_STORE_TOP_BOTTOM((ADDRINT)-1);
		treeTop->incCounter();

		goto before_ret;
	}

	if (globalCtx->options.showCalls)
		funcTraceDebugDump(globalCtx, fc, ctx, reg_sp, treeTop, treeBottom);

	traceObject(fc, globalCtx->kval(), ctx, treeTop, treeBottom, true);
	
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
	kCCFContextClass *globalCtx = globalSharedContext;
	kCCFThreadContextClass *ctx;

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

		ctx->shadowStack.pop();

		if (globalCtx->WorkingMode() == TradMode)
			tradMode_ret();
	}

#endif

	if (ctx->shadowStack.size() == 1) {
		
		dbg_funcret_pop_err();
		goto function_end;
	}
	
	dbg_funcret_pop();

	ctx->shadowStack.pop();

	dbg_funcret_stack_after_pop();

	dbg_funcret_new_call();


function_end:

	if (globalCtx->WorkingMode() == TradMode)
		tradMode_ret();

	ctx->setCurrentFunction(ctx->shadowStack.top().treeTop->getValue()->getKey());
}
