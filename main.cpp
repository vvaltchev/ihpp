/*
	============================================================================

		IHPP: An Intraprocedural Hot Path Profiler

		Author: Vladislav K. Valtchev (vladi32@gmail.com)





		The kSlabForest and kCCF data structures, 
		the forest join operation, the forest inverseK operation 
		and the traceObject function (and only these ones),
		are based on the theoretical research of: 

			Giorgio Ausiello (ausiello@dis.uniroma1.it)
			Camil Demetrescu (demetres@dis.uniroma1.it)
			Irene Finocchi (finocchi@di.uniroma1.it)
			Donatella Firmani (firmani@dis.uniroma1.it)
	
		Universita' degli studi di Roma "La Sapienza"


    ============================================================================

	Licence: LGPL (see LICENCE.txt)

*/

#define MAIN_IHPP_MODULE

using namespace std;

#include "config.h"
#include "debug.h"
#include "ihpp.h"
#include "benchmark.h"
#include "output.h"

#include "tracingFuncs.h"


VOID blockModeBlockTrace(TracingObject<ADDRINT> *to) { 

	ihppThreadContextClass *ctx;
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
	//string funcName;
	INT32 row,col;
	BasicBlock *bb;
	ADDRINT blockPtr,funcAddr;
	map<ADDRINT,BasicBlock*>::iterator it;

	ihppContextClass *ctx = globalSharedContext;

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


		//funcName=RTN_Name(rtn);
		funcAddr=RTN_Address(rtn);

		/*
		if (ctx->options.purgeFuncs) {
				
			if (funcName[0] == '_') {
				PIN_UnlockClient();
				continue;
			}
		}
		*/


		if (!ctx->hasToTrace(funcAddr)) {

			PIN_UnlockClient();
			continue;
		}

		it = ctx->allBlocks.find(blockPtr);

		if (it == ctx->allBlocks.end()) {

			INS ins = BBL_InsTail(bbl);
			ADDRINT lastAddr = INS_Address(ins);

			assert(ctx->allFuncs.find(funcAddr) != ctx->allFuncs.end());

			bb = new BasicBlock(blockPtr, ctx->allFuncs.find(funcAddr)->second, lastAddr, row, col); 
			ctx->allBlocks[blockPtr]=bb;

		} else {
				
			bb = it->second;
		}


		if (ctx->WorkingMode() == BlockMode)
			BBL_InsertCall(bbl, 
								IPOINT_BEFORE, AFUNPTR(blockModeBlockTrace), 
								IARG_PTR, bb, 
								IARG_END);		

		if (ctx->WorkingMode() == IntraMode)
			BBL_InsertCall(bbl, 
								IPOINT_BEFORE, AFUNPTR(intraModeBlockTrace), 
								IARG_CALL_ORDER, CALL_ORDER_LAST, 
								IARG_PTR, bb, 
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

void imageLoad_doInsInstrumentation(IMG &img, RTN &rtn, FunctionObj *fc) {

	ihppContextClass *ctx = globalSharedContext;

	RTN_Open(rtn);

	for( INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins) ) {
						
		if (ctx->WorkingMode() != BlockMode)
			insInstrumentation(rtn, ins);			

		if (!ctx->options.blocksDisasm && !ctx->options.funcsDisasm)
			continue;

		string ins_text = INS_Disassemble(ins);

		if (!INS_IsDirectBranchOrCall(ins)) {

			fc->instructions[INS_Address(ins)] = insInfo(duplicate_string(ins_text), 0, 0);
			continue;
		}
					
		ADDRINT addr = INS_DirectBranchOrCallTargetAddress(ins);
		RTN r = RTN_FindByAddress(addr);
			
		if (!RTN_Valid(r)) {
		
			fc->instructions[INS_Address(ins)] = insInfo(duplicate_string(ins_text), addr, 0);
			continue;
		}


		string rname = RTN_Name(r);
		
		
		insInfo insData = insInfo(duplicate_string(ins_text), addr, RTN_Address(r));

		RTN r2 = RTN_FindByName(img, rname.c_str());

		if (!RTN_Valid(r2) && !FUNC_IS_TEXT(RTN_Address(r))) {	

			insData.externFuncName = duplicate_string(rname);
			fc->instructions[INS_Address(ins)] = insData;
			continue;
		} 

		fc->instructions[INS_Address(ins)] = insData;
	}

	RTN_Close(rtn);
}

void ImageLoad(IMG img, void *) {

	RTN rtn2;
	ihppContextClass *ctx = globalSharedContext;
	
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
			string fileName;

			PIN_GetSourceLocation(funcAddr, 0, 0, &fileName);

			imageload_specialfunc(funcAddr, funcName);
							
#ifdef __unix__

			if (SEC_Name(RTN_Sec(rtn)) == ".plt")
				continue;

#endif
			/*
			if (ctx->options.purgeFuncs) {
				
				if (funcName[0] == '_' && !IS_WIN32_NLG_NOTIFY(funcAddr)) {
					continue;
				}
			}
			*/
			

			assert(ctx->allFuncs.find(funcAddr) == ctx->allFuncs.end());

			fc = new FunctionObj(funcAddr, funcName, fileName);
			ctx->allFuncs[funcAddr]=fc;
			

			bool trace = ctx->hasToTraceByName(funcName, funcAddr);

			dbg_imgload_funcname();

			

			if (trace || FUNC_IS_TEXT_N(funcName)) {

				imageLoad_doInsInstrumentation(img, rtn, fc);
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

	
	


	if (ctx->options.startFuncName != "--") {
		
		rtn2 = RTN_FindByName(img, ctx->options.startFuncName.c_str());

		if (!RTN_Valid(rtn2)) {
			
			cerr << "Error: start tracing function '" << ctx->options.startFuncName << "' not found.\n";
			exit(0);
		}

		ctx->startFuncAddr = RTN_Address(rtn2);
	}

	if (ctx->options.stopFuncName != "--") {
	
		rtn2 = RTN_FindByName(img, ctx->options.stopFuncName.c_str());


		if (!RTN_Valid(rtn2)) {
		
			cerr << "Error: stop tracing function '" << ctx->options.stopFuncName << "' not found.\n";
			exit(0);
		}

		ctx->stopFuncAddr = RTN_Address(rtn2);
	}


#if defined(_WIN32)

	if (!ctx->spAttrs.text_addr && ctx->spAttrs.unnamedImageEntryPoint_addr)
		ctx->spAttrs.text_addr=ctx->spAttrs.unnamedImageEntryPoint_addr;

#endif
}


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

	globalSharedContext = new ihppContextClass(optionsClass::getGlobalWM(), optionsClass::getGlobalKVal(), options);
	
	KNOB<string> &funcs = optionsClass::tracingFunctions();

	for (unsigned int i=0; i < funcs.NumberOfValues(); i++)
		globalSharedContext->funcsToTrace.insert(funcs.Value(i));

	globalSharedContext->OutFile.open(optionsClass::getOutfileName());
    

	if (globalSharedContext->WorkingMode() != FuncMode) {
		TRACE_AddInstrumentFunction(BlockTraceInstrumentation, 0);
	}

	IMG_AddInstrumentFunction(ImageLoad, 0);
	

    PIN_AddFiniFunction(Fini, 0);

	if (options.kinf) {
	
		traceObject = traceObject_acc_kinf;
	
	} else {
		
		traceObject = traceObject_generic;
	}

    PIN_StartProgram();
    
    return 0;
}
