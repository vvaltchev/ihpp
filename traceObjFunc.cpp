
#include "ihpp.h"

#include "tracingFuncs.h"
#include "benchmark.h"


void (*traceObject)(TracingObject<ADDRINT> *to,
                    GenericTraceContext *ctx,
                    ihppNode* &treeTop,
                    ihppNode* &treeBottom) = nullptr;

void traceObject_kinf_roll(TracingObject<ADDRINT> *to,
                           GenericTraceContext *ctx,
                           ihppNode* &treeTop,
                           ihppNode* &treeBottom)
{
    ihppNode *n;
    ADDRINT key = to->getKey();


    if (!treeTop) {

        treeTop = ctx->kSlabForest.addTreeByVal(ihppNode(key, to, 1));

        if (!ctx->rootKey)
            ctx->rootKey=key;

    } else {

        //roll simple rec
        if (to == treeTop->getValue()) {

            treeTop->incCounter();
            return;
        }

        n = treeTop->getChildRef(key);

        if (!n)
            n = treeTop->addChildByVal(ihppNode(key, to, 1));
        else
            n->incCounter();

        treeTop=n;
    }

}

void traceObject_kinf_no_roll(TracingObject<ADDRINT> *to,
                              GenericTraceContext *ctx,
                              ihppNode* &treeTop,
                              ihppNode* &treeBottom)
{
    ihppNode *n;
    ADDRINT key = to->getKey();

    if (!treeTop) {

        treeTop = ctx->kSlabForest.addTreeByVal(ihppNode(key, to, 1));

        if (!ctx->rootKey)
            ctx->rootKey=key;

    } else {

        n = treeTop->getChildRef(key);

        if (!n)
            n = treeTop->addChildByVal(ihppNode(key, to, 1));
        else
            n->incCounter();

        treeTop=n;
    }

}

void traceObject_classic_roll(TracingObject<ADDRINT> *to,
                              GenericTraceContext *ctx,
                              ihppNode* &treeTop,
                              ihppNode* &treeBottom)
{

    ihppNode *n;
    ihppNodeMap::iterator it;
    ADDRINT key = to->getKey();
    unsigned int k = globalSharedContext->kval();

    //roll simple rec
    if (treeTop && to == treeTop->getValue()) {

        treeTop->incCounter();
        return;
    }

    bool mod0 = !(ctx->counter % k);

    if (to->getType() == TracingObjType::Func)
        mod0 = !(ctx->shadowStack.size() % k);

    // For k -> inf, this code runs only first time, when counter=0
    if (mod0) {

        if (!treeTop && !treeBottom)
            ctx->rootKey=key;

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

void traceObject_classic_no_roll(TracingObject<ADDRINT> *to,
                                 GenericTraceContext *ctx,
                                 ihppNode* &treeTop,
                                 ihppNode* &treeBottom)
{
    ihppNode *n;
    ihppNodeMap::iterator it;
    ADDRINT key = to->getKey();
    unsigned int k = globalSharedContext->kval();

    bool mod0 = !(ctx->counter % k);

    if (to->getType() == TracingObjType::Func)
        mod0 = !(ctx->shadowStack.size() % k);

    // For k -> inf, this code runs only first time, when counter=0
    if (mod0) {

        if (!treeTop && !treeBottom)
            ctx->rootKey=key;

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

