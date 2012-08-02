
#ifndef __TRACING_FUNCS__
#define __TRACING_FUNCS__

#include "pin.H"
#include "tracingObjects.h"



VOID FunctionObjTrace(FunctionObj *fc, ADDRINT reg_sp);

void funcMode_ret();


VOID intraModeBlockTrace(TracingObject<ADDRINT> *to, ADDRINT reg_sp);

void intraMode_ret();


#if ENABLE_INS_TRACING

void PIN_FAST_ANALYSIS_CALL singleInstruction(ADDRINT currFuncAddr);


void branchOrCall(ADDRINT currentFuncAddr, ADDRINT targetAddr, 
							ADDRINT targetFuncAddr, ADDRINT insCat);

void indirect_branchOrCall(ADDRINT currentFuncAddr,	
								ADDRINT targetAddr, ADDRINT insCat);

#endif



void traceObject_generic(TracingObject<ADDRINT> *to, 
										ihppAbstractContext *ctx, ihppNode* &treeTop, 
										ihppNode* &treeBottom);

void traceObject_acc_kinf(TracingObject<ADDRINT> *to, 
										ihppAbstractContext *ctx, ihppNode* &treeTop, 
										ihppNode* &treeBottom);


extern void (*traceObject)(TracingObject<ADDRINT> *to, 
										ihppAbstractContext *ctx, ihppNode* &treeTop, 
										ihppNode* &treeBottom);


#endif