#pragma once

#include "pin.H"
#include "tracingObjects.h"


//FuncMode and IntraMode tracing functions

#if ENABLE_KEEP_STACK_PTR
    void FunctionObjTrace(FunctionObj *fc, ADDRINT reg_sp);
    void intraModeBlockTrace(BasicBlock *bb, ADDRINT reg_sp);
#else
    void FunctionObjTrace(FunctionObj *fc);
    void intraModeBlockTrace(BasicBlock *bb);
#endif

void intraMode_ret();
void funcMode_ret();

//InterProcMode tracing functions

void interModeBlockTrace(BasicBlock *bb);

//Instruction-tracing functions

#if ENABLE_INS_TRACING

void PIN_FAST_ANALYSIS_CALL singleInstruction(ADDRINT currFuncAddr);


void branchOrCall(ADDRINT currentFuncAddr,
                  ADDRINT targetAddr,
                  ADDRINT targetFuncAddr,
                  ADDRINT insCat);

void indirect_branchOrCall(ADDRINT currentFuncAddr,
                           ADDRINT targetAddr,
                           ADDRINT insCat);

#endif

//The main traceObject function


void traceObject_classic_roll(TracingObject<ADDRINT> *to,
                              GenericTraceContext *ctx, ihppNode* &treeTop,
                              ihppNode* &treeBottom);

void traceObject_classic_no_roll(TracingObject<ADDRINT> *to,
                                 GenericTraceContext *ctx, ihppNode* &treeTop,
                                 ihppNode* &treeBottom);

void traceObject_kinf_roll(TracingObject<ADDRINT> *to,
                           GenericTraceContext *ctx, ihppNode* &treeTop,
                           ihppNode* &treeBottom);

void traceObject_kinf_no_roll(TracingObject<ADDRINT> *to,
                              GenericTraceContext *ctx, ihppNode* &treeTop,
                              ihppNode* &treeBottom);

extern void (*traceObject)(TracingObject<ADDRINT> *to,
                           GenericTraceContext *ctx, ihppNode* &treeTop,
                           ihppNode* &treeBottom);
