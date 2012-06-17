/*
	============================================================================

		k-Calling Context Forest profiling pintool

		Author: Vladislav K. Valtchev (vladi32@gmail.com)





		The kSlabForest and kCCF data structures, 
		the forest join operation, the forest inverseK operation 
		and the traceObject function (and only these ones),
		are based on the theoretical research of: 

			Giorgio Ausiello (ausiello@dis.uniroma1.it)
			Camil Demetrescu (demetres@dis.uniroma1.it)
			Irene Finocchi (finocchi@di.uniroma1.it)
			Donatella Firmani (firmani@dis.uniroma1.it)
	
		Sapienza University of Rome


    ============================================================================

	Licence: LGPL (see LICENCE.txt)

*/

#define MAIN_KCCF_MODULE

#include "config.h"
#include "debug.h"
#include "kCCF.h"

using namespace std;

#include "benchmark.h"
#include "output.h"

#include "tracingFuncs.h"


VOID blockModeBlockTrace(TracingObject<ADDRINT> *to) { 

	kCCFThreadContextClass *ctx;
	BasicBlock *bb = static_cast<BasicBlock*>(to);

	ctx = globalSharedContext->getThreadCtx(PIN_ThreadUid());

	if (bb->functionAddr() == ctx->startFuncAddr)
		ctx->haveToTrace=true;

	if (!ctx->haveToTrace)
		return;	

	if (bb->functionAddr() == ctx->stopFuncAddr)
		ctx->haveToTrace=false;


	bb->incSimpleCounter();

	traceObject_generic(bb, ctx, ctx->treeTop, ctx->treeBottom);
}


void insInstrumentation(RTN rtn, INS ins) {


	if (INS_IsRet(ins) && !FUNC_IS_TEXT(RTN_Address(rtn))) {

		INS_InsertPredicatedCall(ins, 
									IPOINT_BEFORE, (AFUNPTR)funcMode_ret, 
									IARG_CALL_ORDER,
									CALL_ORDER_FIRST, 
									IARG_END);

	}

#if ENABLE_INS_TRACING

	if ((INS_IsBranchOrCall(ins) || INS_IsRet(ins))) {
		
		if (INS_IsDirectBranchOrCall(ins)) {
		
			ADDRINT targetAddr = INS_DirectBranchOrCallTargetAddress(ins);

			RTN r;
			PIN_LockClient();
			r = RTN_FindByAddress(targetAddr);
			PIN_UnlockClient();
			

			if (!RTN_Valid(r)) {
			
				return;
			}

			INS_InsertPredicatedCall(ins, 
										IPOINT_BEFORE, (AFUNPTR)branchOrCall, 
										IARG_CALL_ORDER,
										CALL_ORDER_FIRST,
										IARG_PTR, RTN_Address(rtn),
										
										IARG_PTR, targetAddr,
										IARG_PTR, RTN_Address(r),
										
										IARG_PTR, INS_Category(ins),
										IARG_END);

		} else {

			INS_InsertPredicatedCall(ins, 
										IPOINT_BEFORE, (AFUNPTR)indirect_branchOrCall, 
										IARG_CALL_ORDER,
										CALL_ORDER_FIRST,
										IARG_PTR, RTN_Address(rtn),
										
										IARG_BRANCH_TARGET_ADDR,

										IARG_PTR, INS_Category(ins),
										IARG_END);		

		}


	} else {
		INS_InsertPredicatedCall(ins, 
									IPOINT_BEFORE, (AFUNPTR)singleInstruction, 
									IARG_CALL_ORDER,
									CALL_ORDER_FIRST, 
									IARG_FAST_ANALYSIS_CALL, 
									IARG_PTR, RTN_Address(rtn), 
									IARG_END);
	}
#endif

}


VOID BlockTraceInstrumentation(TRACE trace, void *)
{
    
	RTN rtn;
	string file;
	string funcName;
	INT32 row,col;
	BasicBlock *bb;
	ADDRINT blockPtr,funcAddr;
	map<ADDRINT,BasicBlock*>::iterator it;

	kCCFContextClass *ctx = globalSharedContext;

    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
	
		PIN_LockClient();
 
		blockPtr = BBL_Address(bbl);

		PIN_GetSourceLocation(blockPtr, &col, &row, 0);
	 	
		rtn=RTN_FindByAddress(blockPtr);

		if (!RTN_Valid(rtn)) {

			PIN_UnlockClient();
			continue;
		}


		funcName=RTN_Name(rtn);
		funcAddr=RTN_Address(rtn);

		if (ctx->options.purgeFuncs) {
				
			if (funcName[0] == '_') {
				PIN_UnlockClient();
				continue;
			}
		}


		//if (!ctx->hasToTraceByName(funcName, funcAddr)) {
			
		if (!ctx->hasToTrace(funcAddr)) {

			PIN_UnlockClient();
			continue;
		}

		it = ctx->allBlocks.find(blockPtr);

		if (it == ctx->allBlocks.end()) {

			bb = new BasicBlock(blockPtr, ctx->allFuncs.find(funcAddr)->second, row, col); 
			ctx->allBlocks[blockPtr]=bb;

			if (ctx->options.disasm) 
			{
				for( INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins) )
				{
					if (INS_IsDirectCall(ins)) {
				
						ADDRINT addr = INS_DirectBranchOrCallTargetAddress(ins);
						RTN r = RTN_FindByAddress(addr);
					
						if (RTN_Valid(r)) {
						
							bb->instructions.push_back("call "+RTN_Name(r));
							continue;	
						}
					}

					bb->instructions.push_back(INS_Disassemble(ins));
				}
			}

		} else {
				
			bb = it->second;
		}


		if (ctx->WorkingMode() == BlockMode)
			BBL_InsertCall(bbl, 
								IPOINT_BEFORE, AFUNPTR(blockModeBlockTrace), 
								IARG_PTR, bb, 
								//IARG_PTR, ctx, 
								IARG_END);		

		if (ctx->WorkingMode() == TradMode)
			BBL_InsertCall(bbl, 
								IPOINT_BEFORE, AFUNPTR(tradModeBlockTrace), 
								IARG_CALL_ORDER, CALL_ORDER_LAST, 
								IARG_PTR, bb, 
								//IARG_PTR, ctx, 
								IARG_REG_VALUE, REG_STACK_PTR, 
								IARG_END);		

		PIN_UnlockClient();
    }
}

inline void imageload_specialfunc(ADDRINT &funcAddr, string &funcName) {

#if defined(_WIN32)

	if (funcName == "_NLG_Notify")
		globalSharedContext->spAttrs._NLG_Notify_addr = funcAddr;
	else
	if (funcName == "_NLG_Notify1")
		globalSharedContext->spAttrs._NLG_Notify1_addr = funcAddr;
	else
	if (funcName == "__NLG_Dispatch")
		globalSharedContext->spAttrs.__NLG_Dispatch_addr = funcAddr;
	else
	if (funcName == "__tmainCRTStartup")
		globalSharedContext->spAttrs.__tmainCRTStartup_addr = funcAddr;
	else
	if (funcName == "wWinMain")
		globalSharedContext->spAttrs.wWinMain_addr = funcAddr;
	else
	if (funcName == "unnamedImageEntryPoint")
		globalSharedContext->spAttrs.unnamedImageEntryPoint_addr = funcAddr;

#endif

	if (funcName == "main")
		globalSharedContext->spAttrs.main_addr = funcAddr;
	else
	if (funcName == ".text")
		globalSharedContext->spAttrs.text_addr = funcAddr;
}

VOID ImageLoad(IMG img, VOID *) {

	RTN rtn2;
	kCCFContextClass *ctx = globalSharedContext;
	
	//ctx->showDebug=false;

	FunctionObj *fc;
	map<ADDRINT, FunctionObj*>::iterator it;
	bool mainImage = IMG_IsMainExecutable(img);
	
	dbg_imgload_imgname();

	rtn2 = RTN_FindByName(img, "exit");

	if (RTN_Valid(rtn2)) {
	
		globalSharedContext->spAttrs.exit_addr = RTN_Address(rtn2);
	}

	if (!mainImage)
		return;


	for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec)) {

		dbg_imgload_sectorname();

		for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn)) {
			

			ADDRINT funcAddr = RTN_Address(rtn);
			string funcName = RTN_Name(rtn);

			imageload_specialfunc(funcAddr, funcName);
							
#ifdef __unix__

			if (SEC_Name(RTN_Sec(rtn)) == ".plt")
				continue;

#endif

			if (ctx->options.purgeFuncs) {
				
				if (funcName[0] == '_' && !IS_WIN32_NLG_NOTIFY(funcAddr)) {
					continue;
				}
			}

			

			assert(ctx->allFuncs.find(funcAddr) == ctx->allFuncs.end());

			fc = new FunctionObj(funcAddr, funcName);
			ctx->allFuncs[funcAddr]=fc;
			

			bool trace = ctx->hasToTraceByName(funcName, funcAddr);

			dbg_imgload_funcname();

			

			if (trace || FUNC_IS_TEXT_N(funcName)) {
				
				if (ctx->WorkingMode() != BlockMode) {
				
					RTN_Open(rtn);

					for( INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins) )
						insInstrumentation(rtn, ins);			

					RTN_Close(rtn);
				}
			}

			if (!trace)
				continue;

			ctx->funcAddrsToTrace.insert(funcAddr);
				
			if (ctx->WorkingMode() != BlockMode)
			{	
				RTN_Open(rtn);
				RTN_InsertCall(rtn, IPOINT_BEFORE, AFUNPTR(FunctionObjTrace), 
									IARG_CALL_ORDER, CALL_ORDER_FIRST, 
									IARG_PTR, (ADDRINT)fc, 
									IARG_PTR, ctx, 
									IARG_REG_VALUE, REG_STACK_PTR, 
									IARG_END);
				RTN_Close(rtn);
			}

		}
	}

	
	


	if (ctx->options.startFuncName != "-none-") {
		
		rtn2 = RTN_FindByName(img, ctx->options.startFuncName.c_str());

		if (!RTN_Valid(rtn2)) {
			
			cerr << "Error: start tracing function '" << ctx->options.startFuncName << "' not found.\n";
			exit(0);
		}

		ctx->startFuncAddr = RTN_Address(rtn2);
	}

	if (ctx->options.stopFuncName != "-none-") {
	
		rtn2 = RTN_FindByName(img, ctx->options.stopFuncName.c_str());


		if (!RTN_Valid(rtn2)) {
		
			cerr << "Error: stop tracing function '" << ctx->options.stopFuncName << "' not found.\n";
			exit(0);
		}

		ctx->stopFuncAddr = RTN_Address(rtn2);
	}
	
}


//#include "experimentalCode.h"


/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */


int main(int argc, char ** argv) {

	PIN_InitSymbols();

	if (PIN_Init(argc, argv)) {
	
		optionsClass::showHelp();
		return 0;
	}

	if (!optionsClass::checkOptions()) {
	
		return 0;
	}


	optionsClass options;

	options.initFromGlobalOptions();

	globalSharedContext = new kCCFContextClass(optionsClass::getGlobalWM(), optionsClass::getGlobalKVal(), options);
	
	KNOB<string> &funcs = optionsClass::tracingFunctions();

	for (unsigned int i=0; i < funcs.NumberOfValues(); i++)
		globalSharedContext->funcsToTrace.insert(funcs.Value(i));

	globalSharedContext->OutFile.open(optionsClass::getOutfileName());
    

	if (globalSharedContext->WorkingMode() != FuncMode) {
		TRACE_AddInstrumentFunction(BlockTraceInstrumentation, 0);
	}

	IMG_AddInstrumentFunction(ImageLoad, 0);
	
	//Experimental only imageload function, don't use it
	//IMG_AddInstrumentFunction(ImageLoad2, (void*)globalSharedContext);

    PIN_AddFiniFunction(Fini, 0);

	if (options.kinf) {
	
		traceObject = traceObject_acc_kinf;
	
	} else {
		
		traceObject = traceObject_generic;
	}

    PIN_StartProgram();
    
    return 0;
}
