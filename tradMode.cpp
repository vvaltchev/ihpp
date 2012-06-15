

#include "config.h"
#include "debug.h"
#include "kCCF.h"

using namespace std;
using namespace kCCFLib;

#include "benchmark.h"
#include "output.h"

#include "tracingFuncs.h"

#define TRADMODE_LOAD_TOP_BOTTOM()					treeTop = tradCtx->shadowStack.top().treeTop; treeBottom = tradCtx->shadowStack.top().treeBottom; 

#define TRADMODE_STORE_TOP_BOTTOM(sp)				tradCtx->shadowStack.push(ShadowStackType(treeTop,treeBottom, (sp) ));


#define TRADMODE_REPLACE_TOP_BOTTOM(sp)				tradCtx->shadowStack.pop(); tradCtx->shadowStack.push(ShadowStackType(treeTop,treeBottom, (sp) ));

#define TOP_STACKPTR()								(tradCtx->shadowStack.size()?tradCtx->shadowStack.top().stackPtr:(ADDRINT)-1)

#define TRADMODE_SET_TOP_BOTTOM_TO_ROOT()			tradCtx->counter=1; treeTop=tradCtx->kSlabForest.getTreeRef(tradCtx->rootKey); treeBottom=0;
#define TRADMODE_TOP_BOTTOM_ARE_POINTING_TO_ROOT()	(treeTop==tradCtx->kSlabForest.getTreeRef(tradCtx->rootKey) && !treeBottom)


VOID tradModeBlockTrace(TracingObject<ADDRINT> *to, ADDRINT reg_sp) { 

	kCCFContextClass *globalCtx = globalSharedContext;
	kCCFThreadContextClass *ctx;
	kCCFTradModeContext *tradCtx;
	kCCFNode *treeTop;
	kCCFNode *treeBottom;

	BasicBlock *bb = static_cast<BasicBlock *>(to);

	bb->incSimpleCounter();

	ctx = globalCtx->getThreadCtx(PIN_ThreadUid());

	
	if (!ctx->haveToTrace)
		return;

	if ( FUNC_IS_TEXT(bb->functionName()) )
		return;

	
#ifdef _WIN32

	//string func = bb->functionName();

	if (IS_WIN32_NLG_NOTIFY(bb->functionAddr())) {

		dbg_tradtr_nlog_skip();
		return;
	}

#endif
	
	dbg_tradtr_begin();
	
#if ENABLE_RELY_ON_SP_CHECK
	
	ctx->setCurrentFunction(bb->functionAddr());

#else

	if ( ctx->getCurrentFunction() && ctx->getCurrentFunction() != bb->functionAddr() ) {
	
		dbg_tradtr_longjmp();
		
		tradMode_ret();
		ctx->setCurrentFunction(bb->functionAddr());
	}

#endif	
	
	tradCtx = ctx->getCurrentFunctionCtx();

	dbg_tradtr_begin_sp();

	if (!bb->isFirstBlock()) {
	
		//If this function wasn't traced before (due to startFunc), don't it trace now..
		if (!tradCtx->shadowStack.size())
			return;

		dbg_tradtr_normal_trace();

		TRADMODE_LOAD_TOP_BOTTOM();

		ADDRINT oldStackPtr = TOP_STACKPTR();

		if (!globalCtx->options.rollLoops) {
			
			traceObject(bb, globalCtx->kval(), tradCtx, treeTop, treeBottom, true);

		} else {
		
			bool found=false;
			kCCFNode *parent = treeTop->getParentRef();

			while (parent) {
			
			
				if (parent->getKey() == bb->getKey()) {
				
					treeTop=parent;
					treeBottom=0;
					tradCtx->counter=1;
					found=true;
					treeTop->incCounter();
					break;
										
				}

				parent = parent->getParentRef();
			}

			if (!found) {
				
				traceObject(bb, globalCtx->kval(), tradCtx, treeTop, treeBottom, true);
			} 
		}

		TRADMODE_REPLACE_TOP_BOTTOM(oldStackPtr);

		dbg_tradtr_end_sp();

		return;
	}

	/*
		First block of a function is met: it has the SAME address as the function.
		This happens where there is a RECURSION or a NORMAL CALL of the function (maybe also the first time)
	*/
	dbg_tradtr_first_block();

	

	if (!tradCtx->rootKey) {
		
		/*
			Rootkey is null, so this is the first the function is called (in this thread): everything is very simple.
		*/

		dbg_tradtr_first_call();
			
		treeTop=0; treeBottom=0;
				
		traceObject(bb, globalCtx->kval(), tradCtx, treeTop, treeBottom, true);

		assert(tradCtx->rootKey);
		assert(treeTop);
				
		TRADMODE_STORE_TOP_BOTTOM(reg_sp);
		return;
	}

	
	//This was NOT the first time something called this function.
	
	TRADMODE_LOAD_TOP_BOTTOM();

	if (!TRADMODE_TOP_BOTTOM_ARE_POINTING_TO_ROOT()) 
	{
		//Reset top,bottom pointers to root of the function's tree
		TRADMODE_SET_TOP_BOTTOM_TO_ROOT();

		//Push them on the shadow stack a increment the root counter
		TRADMODE_STORE_TOP_BOTTOM(reg_sp);
	}

	treeTop->incCounter();

	dbg_tradtr_end_sp();
}



void PIN_FAST_ANALYSIS_CALL tradMode_ret() 
{
	kCCFThreadContextClass *ctx;
	kCCFTradModeContext *tradCtx;

	kCCFContextClass *globalCtx = globalSharedContext;
	ctx = globalCtx->getThreadCtx(PIN_ThreadUid());


	tradCtx = ctx->getCurrentFunctionCtx();


#ifdef _WIN32
	if (IS_WIN32_NLG_NOTIFY(ctx->getCurrentFunction()))
		return;
#endif

	dbg_tradret_begin();

	if (!tradCtx->shadowStack.size()) {
	
		dbg_tradret_cantpop();
		return;
	}

	dbg_tradret_stackpop();

	tradCtx->shadowStack.pop();

	if (!tradCtx->shadowStack.size()) {
		
		kCCFNode *treeTop;
		kCCFNode *treeBottom;
	
		dbg_tradret_lastret();

		/*
			Since now shadow stack is empty, on the next activation of the function
			top and bottom pointers MUST point to the calling tree's root.
		*/

		//Reset top,bottom pointers to root of the function's tree
		TRADMODE_SET_TOP_BOTTOM_TO_ROOT();

		//Push them on the shadow stack a increment the root counter
		TRADMODE_STORE_TOP_BOTTOM((ADDRINT)-1);		
	}
}
