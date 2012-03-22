

VOID FunctionObjTrace(FunctionObj *fc, kCCFContextClass *globalCtx, ADDRINT reg_sp);

void PIN_FAST_ANALYSIS_CALL funcMode_ret(kCCFContextClass *globalCtx);

inline void funcMode_Ret_wrapper() { funcMode_ret(globalSharedContext); }


VOID tradModeBlockTrace(TracingObject<ADDRINT> *to, kCCFContextClass *globalCtx, ADDRINT reg_sp);

void PIN_FAST_ANALYSIS_CALL tradMode_ret(kCCFContextClass *globalCtx);


#if ENABLE_INS_TRACING

void PIN_FAST_ANALYSIS_CALL singleInstruction(kCCFContextClass *globalCtx, ADDRINT currFuncAddr, ADDRINT insCat);

void branchOrCall(kCCFContextClass *globalCtx, ADDRINT insAddr, ADDRINT targetAddr, ADDRINT insCat);

#endif


namespace kCCFLib {

template <typename T>
void traceObject(TracingObject<T> *to, int k, 
											kCCFAbstractContext *ctx, kCCFNode* &treeTop, 
											kCCFNode* &treeBottom, bool accumulate=false) 
{

	kCCFNode *n;
	kCCFNodeMap::iterator it;
	ADDRINT key = to->getKey();


	if (!treeTop && !treeBottom) {
	
		ctx->rootKey=key;
	}

	/*
		When accumulate is true, consecutive runs of the same object, say A, 
		will increment it's counter instead of adding a node A as child of A.
		For example, with accumulate=true, this IS NOT possibile:
	
		A,1
		   | A,1
			  | A,1

		That calling sequence will became:
		A,3
	*/
	if (accumulate) {
	
		
		if (treeTop && to == treeTop->getValue()) {
	
			treeTop->incCounter();
			return;
		}

	}

	if (globalSharedContext->kinf) {
	
		if (!treeTop) {
			
			treeTop = ctx->kSlabForest.addTreeByVal(kCCFNode(key, to, 1));

		} else {

			n = treeTop->getChildRef(key);
			
			if (!n)
				n = treeTop->addChildByVal(kCCFNode(key, to, 1));
			else
				n->incCounter();

			treeTop=n;
		}

		return;
	}

	if (!k) {
	
		n = ctx->kSlabForest.getTreeRef(key);

		if (n) 
			n->incCounter();
		else 
			ctx->kSlabForest.addTreeByVal(kCCFNode(key,to,1));
		
		return;
	}

	// For k -> inf, this code runs only first time, when counter=0
	if (!( (ctx->counter) % k )) {

		treeBottom=treeTop;
		it = ctx->R.find(key);

		if (it == ctx->R.end())
			ctx->R[key] = ( treeTop = ctx->kSlabForest.addTreeByVal(kCCFNode(key, to, 0))  );
		else
			treeTop = it->second;
	
	} else {
		
		//For k -> inf, only this code runs

		n = treeTop->getChildRef(key);

		if (!n)
			n = treeTop->addChildByVal(kCCFNode(key, to, 0));
		
		treeTop=n;
	}
	

	treeTop->incCounter();

	//For k -> inf, this NEVER runs
	if (treeBottom) {
	
		n = treeBottom->getChildRef(key);

		if (!n)
			n = treeBottom->addChildByVal(kCCFNode(key, to, 0));
		
		treeBottom=n;
		treeBottom->incCounter();
	}

	ctx->counter++;
}

}