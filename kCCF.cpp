/*
	============================================================================

		k-Calling Context Forest profiling pintool

		Author: Vladislav K. Valtchev (vladi32@gmail.com)
		Date: March, 2012

		Based on the research of: 
			Giorgio Ausiello (ausiello@dis.uniroma1.it)
			Camil Demetrescu (demetres@dis.uniroma1.it)
			Irene Finocchi (finocchi@di.uniroma1.it)
			Donatella Firmani (firmani@dis.uniroma1.it)
	
		Sapienza University of Rome


    ============================================================================

			GNU LESSER GENERAL PUBLIC LICENSE
				Version 3, 29 June 2007

	 Copyright (C) 2007 Free Software Foundation, Inc. <http://fsf.org/>
	 Everyone is permitted to copy and distribute verbatim copies
	 of this license document, but changing it is not allowed.


	  This version of the GNU Lesser General Public License incorporates
	the terms and conditions of version 3 of the GNU General Public
	License, supplemented by the additional permissions listed below.

	  0. Additional Definitions.

	  As used herein, "this License" refers to version 3 of the GNU Lesser
	General Public License, and the "GNU GPL" refers to version 3 of the GNU
	General Public License.

	  "The Library" refers to a covered work governed by this License,
	other than an Application or a Combined Work as defined below.

	  An "Application" is any work that makes use of an interface provided
	by the Library, but which is not otherwise based on the Library.
	Defining a subclass of a class defined by the Library is deemed a mode
	of using an interface provided by the Library.

	  A "Combined Work" is a work produced by combining or linking an
	Application with the Library.  The particular version of the Library
	with which the Combined Work was made is also called the "Linked
	Version".

	  The "Minimal Corresponding Source" for a Combined Work means the
	Corresponding Source for the Combined Work, excluding any source code
	for portions of the Combined Work that, considered in isolation, are
	based on the Application, and not on the Linked Version.

	  The "Corresponding Application Code" for a Combined Work means the
	object code and/or source code for the Application, including any data
	and utility programs needed for reproducing the Combined Work from the
	Application, but excluding the System Libraries of the Combined Work.

	  1. Exception to Section 3 of the GNU GPL.

	  You may convey a covered work under sections 3 and 4 of this License
	without being bound by section 3 of the GNU GPL.

	  2. Conveying Modified Versions.

	  If you modify a copy of the Library, and, in your modifications, a
	facility refers to a function or data to be supplied by an Application
	that uses the facility (other than as an argument passed when the
	facility is invoked), then you may convey a copy of the modified
	version:

	   a) under this License, provided that you make a good faith effort to
	   ensure that, in the event an Application does not supply the
	   function or data, the facility still operates, and performs
	   whatever part of its purpose remains meaningful, or

	   b) under the GNU GPL, with none of the additional permissions of
	   this License applicable to that copy.

	  3. Object Code Incorporating Material from Library Header Files.

	  The object code form of an Application may incorporate material from
	a header file that is part of the Library.  You may convey such object
	code under terms of your choice, provided that, if the incorporated
	material is not limited to numerical parameters, data structure
	layouts and accessors, or small macros, inline functions and templates
	(ten or fewer lines in length), you do both of the following:

	   a) Give prominent notice with each copy of the object code that the
	   Library is used in it and that the Library and its use are
	   covered by this License.

	   b) Accompany the object code with a copy of the GNU GPL and this license
	   document.

	  4. Combined Works.

	  You may convey a Combined Work under terms of your choice that,
	taken together, effectively do not restrict modification of the
	portions of the Library contained in the Combined Work and reverse
	engineering for debugging such modifications, if you also do each of
	the following:

	   a) Give prominent notice with each copy of the Combined Work that
	   the Library is used in it and that the Library and its use are
	   covered by this License.

	   b) Accompany the Combined Work with a copy of the GNU GPL and this license
	   document.

	   c) For a Combined Work that displays copyright notices during
	   execution, include the copyright notice for the Library among
	   these notices, as well as a reference directing the user to the
	   copies of the GNU GPL and this license document.

	   d) Do one of the following:

		   0) Convey the Minimal Corresponding Source under the terms of this
		   License, and the Corresponding Application Code in a form
		   suitable for, and under terms that permit, the user to
		   recombine or relink the Application with a modified version of
		   the Linked Version to produce a modified Combined Work, in the
		   manner specified by section 6 of the GNU GPL for conveying
		   Corresponding Source.

		   1) Use a suitable shared library mechanism for linking with the
		   Library.  A suitable mechanism is one that (a) uses at run time
		   a copy of the Library already present on the user's computer
		   system, and (b) will operate properly with a modified version
		   of the Library that is interface-compatible with the Linked
		   Version.

	   e) Provide Installation Information, but only if you would otherwise
	   be required to provide such information under section 6 of the
	   GNU GPL, and only to the extent that such information is
	   necessary to install and execute a modified version of the
	   Combined Work produced by recombining or relinking the
	   Application with a modified version of the Linked Version. (If
	   you use option 4d0, the Installation Information must accompany
	   the Minimal Corresponding Source and Corresponding Application
	   Code. If you use option 4d1, you must provide the Installation
	   Information in the manner specified by section 6 of the GNU GPL
	   for conveying Corresponding Source.)

	  5. Combined Libraries.

	  You may place library facilities that are a work based on the
	Library side by side in a single library together with other library
	facilities that are not Applications and are not covered by this
	License, and convey such a combined library under terms of your
	choice, if you do both of the following:

	   a) Accompany the combined library with a copy of the same work based
	   on the Library, uncombined with any other library facilities,
	   conveyed under the terms of this License.

	   b) Give prominent notice with the combined library that part of it
	   is a work based on the Library, and explaining where to find the
	   accompanying uncombined form of the same work.

	  6. Revised Versions of the GNU Lesser General Public License.

	  The Free Software Foundation may publish revised and/or new versions
	of the GNU Lesser General Public License from time to time. Such new
	versions will be similar in spirit to the present version, but may
	differ in detail to address new problems or concerns.

	  Each version is given a distinguishing version number. If the
	Library as you received it specifies that a certain numbered version
	of the GNU Lesser General Public License "or any later version"
	applies to it, you have the option of following the terms and
	conditions either of that published version or of any later version
	published by the Free Software Foundation. If the Library as you
	received it does not specify a version number of the GNU Lesser
	General Public License, you may choose any version of the GNU Lesser
	General Public License ever published by the Free Software Foundation.

	  If the Library as you received it specifies that a proxy can decide
	whether future versions of the GNU Lesser General Public License shall
	apply, that proxy's public statement of acceptance of any version is
	permanent authorization for you to choose that version for the
	Library.

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


#include "experimentalCode.h"


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
