
#include "config.h"
#include "debug.h"
#include "ihpp.h"
#include "benchmark.h"
#include "tracingFuncs.h"

void interModeBlockTrace(BasicBlock *bb) { 

    ThreadContext *ctx;
    
    ctx = globalSharedContext->getThreadCtx(PIN_ThreadUid());

#if EMPTY_ANALYSIS
    return;
#endif

    if (bb->functionAddr() == ctx->startFuncAddr)
        ctx->haveToTrace=true;

    if (!ctx->haveToTrace)
        return;    

    if (bb->functionAddr() == ctx->stopFuncAddr)
        ctx->haveToTrace=false;


    bb->incSimpleCounter();

    if (!globalSharedContext->options.rollLoops) {
    
        traceObject(bb, ctx, ctx->treeTop, ctx->treeBottom);
        return;

    }

    //rollLoops ON
        
    ihppNode *parent = ctx->treeTop->getParentRef();

    while (parent) {


        if (parent->getKey() == bb->getKey()) {

            ctx->treeTop=parent;
            ctx->treeBottom=0;
            ctx->treeTop->incCounter();
            return;
        }

        parent = parent->getParentRef();
    }

    //parent NOT found
    
    traceObject(bb, ctx, ctx->treeTop, ctx->treeBottom);
}