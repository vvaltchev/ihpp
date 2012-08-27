
#ifndef __TRACING_FUNCS__
#define __TRACING_FUNCS__

#include "pin.H"
#include "tracingObjects.h"


//FuncMode tracing functions

void FunctionObjTrace(FunctionObj *fc, ADDRINT reg_sp);
void funcMode_ret();

//IntraMode tracing functions

void intraModeBlockTrace(TracingObject<ADDRINT> *to, ADDRINT reg_sp);
void intraMode_ret();

//InterProcMode tracing functions

void interModeBlockTrace(TracingObject<ADDRINT> *to);

//Instruction-tracing functions

#if ENABLE_INS_TRACING

void PIN_FAST_ANALYSIS_CALL singleInstruction(ADDRINT currFuncAddr);


void branchOrCall(ADDRINT currentFuncAddr, ADDRINT targetAddr, 
							ADDRINT targetFuncAddr, ADDRINT insCat);

void indirect_branchOrCall(ADDRINT currentFuncAddr,	
								ADDRINT targetAddr, ADDRINT insCat);

#endif

//The main traceObject function

void traceObject_generic(TracingObject<ADDRINT> *to, 
										GenericTraceContext *ctx, ihppNode* &treeTop, 
										ihppNode* &treeBottom);

void traceObject_acc_kinf(TracingObject<ADDRINT> *to, 
										GenericTraceContext *ctx, ihppNode* &treeTop, 
										ihppNode* &treeBottom);


extern void (*traceObject)(TracingObject<ADDRINT> *to, 
										GenericTraceContext *ctx, ihppNode* &treeTop, 
										ihppNode* &treeBottom);


#endif