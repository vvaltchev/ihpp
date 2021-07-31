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

        Sapienza University of Rome
        [ Universita' degli studi di Roma "La Sapienza" ]


    ============================================================================

    Licence: LGPL (see LICENCE.txt)

*/

#include <iostream>
using namespace std;

#include "config.h"
#include "debug.h"
#include "ihpp.h"
#include "benchmark.h"
#include "output.h"

#include "tracingFuncs.h"

GlobalContext *globalSharedContext = nullptr;

void insInstrumentation(RTN rtn, INS ins) {


    if (INS_IsRet(ins) && !FUNC_IS_TEXT(RTN_Address(rtn))) {

        INS_InsertPredicatedCall(ins,
                                 IPOINT_BEFORE, (AFUNPTR)funcMode_ret,
                                 IARG_CALL_ORDER,
                                 CALL_ORDER_FIRST,
                                 IARG_END);

    }

#if ENABLE_INS_TRACING

    if (!globalSharedContext->options.insTracing)
        return;

    if (INS_IsControlFlow(ins)) {

        if (INS_IsDirectControlFlow(ins)) {

            ADDRINT targetAddr = INS_DirectControlFlowTargetAddress(ins);

            RTN r;
            PIN_LockClient();
            {
                r = RTN_FindByAddress(targetAddr);
            }
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


void BlockTraceInstrumentation(TRACE trace, void *)
{
    RTN rtn;
    string file;
    INT32 row,col;
    BasicBlock *bb;
    ADDRINT blockPtr,funcAddr;
    map<ADDRINT,BasicBlock*>::iterator it;

    GlobalContext *ctx = globalSharedContext;

    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        PIN_LockClient();

        blockPtr = BBL_Address(bbl);

        PIN_GetSourceLocation(blockPtr, &col, &row, 0);

        rtn = RTN_FindByAddress(blockPtr);

        if (!RTN_Valid(rtn)) {

            PIN_UnlockClient();
            continue;
        }

        funcAddr = RTN_Address(rtn);

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


        if (ctx->WorkingMode() == WM_InterProcMode)
            BBL_InsertCall(bbl,
                           IPOINT_BEFORE, AFUNPTR(interModeBlockTrace),
                           IARG_PTR, bb,
                           IARG_END);

        if (ctx->WorkingMode() == WM_IntraMode)
            BBL_InsertCall(bbl,
                           IPOINT_BEFORE, AFUNPTR(intraModeBlockTrace),
                           IARG_CALL_ORDER, CALL_ORDER_LAST,
                           IARG_PTR, bb,
#if ENABLE_KEEP_STACK_PTR
                           IARG_REG_VALUE, REG_STACK_PTR,
#endif
                           IARG_END);

        PIN_UnlockClient();
    }
}

static void imageload_specialfunc(ADDRINT& funcAddr, const string& funcName)
{

#if defined(_WIN32)

    if (funcName == "_NLG_Notify")
        globalSharedContext->spAttrs._NLG_Notify_addr = funcAddr;
    else if (funcName == "_NLG_Notify1")
        globalSharedContext->spAttrs._NLG_Notify1_addr = funcAddr;
    else if (funcName == "__NLG_Dispatch")
        globalSharedContext->spAttrs.__NLG_Dispatch_addr = funcAddr;
    else if (funcName == "__tmainCRTStartup")
        globalSharedContext->spAttrs.__tmainCRTStartup_addr = funcAddr;
    else if (funcName == "wWinMain")
        globalSharedContext->spAttrs.wWinMain_addr = funcAddr;
    else if (funcName == "unnamedImageEntryPoint")
        globalSharedContext->spAttrs.unnamedImageEntryPoint_addr = funcAddr;

#endif

    if (funcName == "main")
        globalSharedContext->spAttrs.main_addr = funcAddr;
    else if (funcName == ".text")
        globalSharedContext->spAttrs.text_addr = funcAddr;
}

void imageLoad_doInsInstrumentation(IMG &img, RTN &rtn, FunctionObj *fc) {

    GlobalContext *ctx = globalSharedContext;

    RTN_Open(rtn);

    for( INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins) ) {

        if (ctx->WorkingMode() != WM_InterProcMode)
            insInstrumentation(rtn, ins);

        if (!ctx->options.blocksDisasm && !ctx->options.funcsDisasm)
            continue;

        const string& ins_text = INS_Disassemble(ins);

        if (!INS_IsDirectControlFlow(ins)) {

            fc->instructions[INS_Address(ins)] = insInfo(ins_text, 0, 0);
            continue;
        }

        ADDRINT addr = INS_DirectControlFlowTargetAddress(ins);
        RTN r = RTN_FindByAddress(addr);

        if (!RTN_Valid(r)) {

            fc->instructions[INS_Address(ins)] = insInfo(ins_text, addr, 0);
            continue;
        }


        const string& rname = RTN_Name(r);
        insInfo insData = insInfo((ins_text), addr, RTN_Address(r));

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

void ImageLoad(IMG img, void *)
{
    RTN rtn2;
    GlobalContext *ctx = globalSharedContext;

    FunctionObj *fc;
    map<ADDRINT, FunctionObj*>::iterator it;
    const bool mainImage = IMG_IsMainExecutable(img);

    dbg_imgload_imgname();

    rtn2 = RTN_FindByName(img, "exit");

    if (RTN_Valid(rtn2)) {

        globalSharedContext->spAttrs.exit_addr = RTN_Address(rtn2);
    }

    if (!mainImage)
        return;


    for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec)) {

        dbg_imgload_sectionname();

        for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn)) {


            ADDRINT funcAddr = RTN_Address(rtn);
            const string& funcName = RTN_Name(rtn);
            string fileName;

            PIN_GetSourceLocation(funcAddr, 0, 0, &fileName);

            imageload_specialfunc(funcAddr, funcName);

#ifdef __unix__

            if (SEC_Name(RTN_Sec(rtn)) == ".plt")
                continue;

#endif

            assert(ctx->allFuncs.find(funcAddr) == ctx->allFuncs.end());

            fc = new FunctionObj(funcAddr, funcName, fileName);
            ctx->allFuncs[funcAddr]=fc;

            const bool trace = ctx->hasToTraceByName(funcName, funcAddr);

            dbg_imgload_funcname();

            if (trace || FUNC_IS_TEXT_N(funcName)) {
                imageLoad_doInsInstrumentation(img, rtn, fc);
            }

            if (!trace)
                continue;

            ctx->funcAddrsToTrace.insert(funcAddr);

            if (ctx->WorkingMode() != WM_InterProcMode)
            {
                RTN_Open(rtn);
                RTN_InsertCall(rtn, IPOINT_BEFORE, AFUNPTR(FunctionObjTrace),
                                    IARG_CALL_ORDER, CALL_ORDER_FIRST,
                                    IARG_PTR, (ADDRINT)fc,
#if ENABLE_KEEP_STACK_PTR
                                    IARG_REG_VALUE, REG_STACK_PTR,
#endif
                                    IARG_END);
                RTN_Close(rtn);
            }
        }
    }


    if (ctx->options.startFuncName != "--") {

        rtn2 = RTN_FindByName(img, ctx->options.startFuncName.c_str());

        if (!RTN_Valid(rtn2)) {

            cerr << "Error: start tracing function '"
                 << ctx->options.startFuncName
                 << "' not found.\n";

            PIN_ExitProcess(1);
        }

        ctx->startFuncAddr = RTN_Address(rtn2);
    }

    if (ctx->options.stopFuncName != "--") {

        rtn2 = RTN_FindByName(img, ctx->options.stopFuncName.c_str());


        if (!RTN_Valid(rtn2)) {

            cerr << "Error: stop tracing function '"
                 << ctx->options.stopFuncName
                 << "' not found.\n";

            PIN_ExitProcess(1);
        }

        ctx->stopFuncAddr = RTN_Address(rtn2);
    }


#if defined(_WIN32)

    if (!ctx->spAttrs.text_addr && ctx->spAttrs.unnamedImageEntryPoint_addr)
        ctx->spAttrs.text_addr=ctx->spAttrs.unnamedImageEntryPoint_addr;

#endif

}

void ThreadStart(THREADID tid, CONTEXT* ctxt, INT32 flags, VOID* v)
{
    globalSharedContext->createThreadContext(tid);
}

void ThreadFini(THREADID tid, const CONTEXT* ctxt, INT32 code, VOID* v)
{
    globalSharedContext->destroyThreadContext(tid);
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

    if (!optionsClass::checkOptions())
        return 0;

    optionsClass options;

    options.initFromGlobalOptions();

    globalSharedContext = new GlobalContext(
        optionsClass::getGlobalWM(),
        optionsClass::getGlobalKVal(),
        options
    );

    if (!options.tracingFuncList.empty()) {

        vector<string> funcs = splitString(options.tracingFuncList, ',');

        if (funcs.empty()) {

            cerr << "Invalid function list: the argument of -funcs option\n";
            cerr << "must be a list of comma-seperated function names.\n";
            return 0;
        }

        for (const string& f: funcs) {
            globalSharedContext->funcsToTrace.insert(f);
        }
    }

    globalSharedContext->OutFile.open(optionsClass::getOutfileName());

    if (globalSharedContext->WorkingMode() != WM_FuncMode) {
        TRACE_AddInstrumentFunction(BlockTraceInstrumentation, 0);
    }

    IMG_AddInstrumentFunction(ImageLoad, 0);


    PIN_AddFiniFunction(Fini, 0);

    if (options.kinf) {

        if (!options.unrollRec)
            traceObject = traceObject_kinf_roll;
        else
            traceObject = traceObject_kinf_no_roll;

    } else {

        if (!options.unrollRec)
            traceObject = traceObject_classic_roll;
        else
            traceObject = traceObject_classic_no_roll;
    }


    //add a fake __root__ function
    if (globalSharedContext->funcsToTrace.size()) {

        FunctionObj *f = new FunctionObj((ADDRINT)-1, "__root__", "");
        globalSharedContext->allFuncs[f->getKey()] = f;
        globalSharedContext->funcAddrsToTrace.insert(f->functionAddress());

        if (globalSharedContext->WorkingMode() != WM_FuncMode) {

            BasicBlock *bb = new BasicBlock(f->functionAddress(), f, f->functionAddress(), 0, 0);
            globalSharedContext->allBlocks[bb->blockAddress()] = bb;
        }
    }

    PIN_AddThreadStartFunction(ThreadStart, NULL);
    PIN_AddThreadFiniFunction(ThreadFini, NULL);

    globalSharedContext->timer = getMilliseconds();
    PIN_StartProgram();
    return 0;
}
