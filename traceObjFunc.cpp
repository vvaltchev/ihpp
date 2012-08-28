
#include "ihpp.h"

#include "tracingFuncs.h"
#include "benchmark.h"


void (*traceObject)(TracingObject<ADDRINT> *to, GenericTraceContext *ctx, 
										ihppNode* &treeTop, ihppNode* &treeBottom)=0;

void traceObject_acc_kinf(TracingObject<ADDRINT> *to, GenericTraceContext *ctx, 
										ihppNode* &treeTop, ihppNode* &treeBottom)
{
	ihppNode *n;
	ADDRINT key = to->getKey();

	if (!treeTop && !treeBottom) {
	
		ctx->rootKey=key;
	}

	if (treeTop && to == treeTop->getValue()) {
	
		treeTop->incCounter();
		return;
	}

	if (!treeTop) {
			
		treeTop = ctx->kSlabForest.addTreeByVal(ihppNode(key, to, 1));

	} else {

		n = treeTop->getChildRef(key);
			
		if (!n)
			n = treeTop->addChildByVal(ihppNode(key, to, 1));
		else
			n->incCounter();

		treeTop=n;
	}

}

void traceObject_generic(TracingObject<ADDRINT> *to, GenericTraceContext *ctx, 
										ihppNode* &treeTop, ihppNode* &treeBottom) 
{

	ihppNode *n;
	ihppNodeMap::iterator it;
	ADDRINT key = to->getKey();
	unsigned int k = globalSharedContext->kval();

	bool accumulate = !globalSharedContext->options.unrollRec; //&& 
						//globalSharedContext->WorkingMode() != WM_InterProcMode;

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

	if (globalSharedContext->options.kinf) {
	
		if (!treeTop) {
			
			treeTop = ctx->kSlabForest.addTreeByVal(ihppNode(key, to, 1));

		} else {

			n = treeTop->getChildRef(key);
			
			if (!n)
				n = treeTop->addChildByVal(ihppNode(key, to, 1));
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
			ctx->kSlabForest.addTreeByVal(ihppNode(key,to,1));
		
		return;
	}

	// For k -> inf, this code runs only first time, when counter=0
	if (!( (ctx->counter) % k )) {

		treeBottom=treeTop;

		n = ctx->kSlabForest.getTreeRef(key);

		if (!n)
			n = ctx->kSlabForest.addTreeByVal(ihppNode(key,to,0));
		
		treeTop = n;

	} else {
		
		//For k -> inf, only this code runs

		n = treeTop->getChildRef(key);

		if (!n)
			n = treeTop->addChildByVal(ihppNode(key, to, 0));
		
		treeTop=n;
	}
	

	treeTop->incCounter();

	//For k -> inf, this NEVER runs
	if (treeBottom) {
	
		n = treeBottom->getChildRef(key);

		if (!n)
			n = treeBottom->addChildByVal(ihppNode(key, to, 0));
		
		treeBottom=n;
		treeBottom->incCounter();
	}

	
	ctx->counter++;
}

