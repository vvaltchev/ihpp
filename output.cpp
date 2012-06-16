
#include "config.h"
#include "debug.h"
#include "kCCF.h"

using namespace std;
using namespace kCCFLib;

//extern kCCFContextClass *globalSharedContext;

#include "benchmark.h"
#include "output.h"

#include "tracingFuncs.h"

#define OUTPUT_LINE		"=============================================================================================\n"
#define OUTPUT_LINE2	"=============================================================================================\n\n"

#define OUTPUT_PLINE	"---------------------------------------------------------------------------------------------\n"
#define OUTPUT_PLINE2	"---------------------------------------------------------------------------------------------\n\n"


inline void benchmark_dump_before_kCCF(kCCFContextClass *globalCtx, BenchmarkObj *ctx)
{

#if KCCF_BENCHMARK 

	globalCtx->OutFile << endl;
	globalCtx->OutFile << "Before creation of k-CCF\n";
	globalCtx->OutFile << "Nodes created (valid):  " << ctx->nodes_created << endl;
	globalCtx->OutFile << "Nodes created (empty):  " << ctx->empty_nodes_created << endl;
	globalCtx->OutFile << "Nodes copied:           " << ctx->nodes_copied << endl;
	globalCtx->OutFile << endl;

#endif

}

inline void benchmark_dump_after_kCCF(kCCFContextClass *globalCtx, BenchmarkObj *ctx)
{

#if KCCF_BENCHMARK 

	globalCtx->OutFile << "After the creation of k-CCF\n";
	globalCtx->OutFile << "Nodes created (valid):  " << ctx->nodes_created << endl;
	globalCtx->OutFile << "Nodes created (empty):  " << ctx->empty_nodes_created << endl;
	globalCtx->OutFile << "Nodes copied:           " << ctx->nodes_copied << endl;
	globalCtx->OutFile << "Forests copied:         " << ctx->forests_copied << endl << endl;

#endif

}

template <typename T>
inline void print_title(kCCFContextClass *globalCtx, T str)
{
	globalCtx->OutFile << OUTPUT_PLINE;
	globalCtx->OutFile << str << "\n";
	globalCtx->OutFile << OUTPUT_PLINE2;
}


template <typename T>
inline void print_thread_id(kCCFContextClass *ctx, T str)
{
	ctx->OutFile << OUTPUT_LINE;
	ctx->OutFile << OUTPUT_LINE;
	ctx->OutFile << "Thread ID: " << str << "\n";
	ctx->OutFile << OUTPUT_LINE;
	ctx->OutFile << OUTPUT_LINE2;
}


template <typename T>
inline void print_func_name(kCCFContextClass *ctx, T str)
{
	ctx->OutFile << OUTPUT_LINE;
	ctx->OutFile << "Function: " << str << "()\n";
	ctx->OutFile << OUTPUT_LINE;
}


void printContextInfo(kCCFContextClass *globalCtx, kCCFAbstractContext *ctx) {

	kCCFForest *kSFCopy = 0;

	//Operations in this copy shouldn't be counted in benchmark
	//because they are computed only for a visualization propouse:
	//the K slab forest need to copied in order to save it's counter values
	//resetted by kSlabForestKLevelCountersClear() before forest inversion operation
	if (globalCtx->options.showkSF) {
	
		BENCHMARK_OFF

		kSFCopy = new kCCFForest;
		*kSFCopy=ctx->kSlabForest;

		BENCHMARK_ON

	}


	globalCtx->OutFile << "Nodes count of k slab forest: " << ctx->kSlabForest.recursiveAllNodesCount() << endl;


	benchmark_dump_before_kCCF(globalCtx, ctx);

	kCCFForest kccf;

	if (globalCtx->options.showkCCF) {
	
		kSlabForestKLevelCountersClear(ctx->kSlabForest, ctx->rootKey, globalCtx->kval());
		kccf = ctx->kSlabForest.inverseK(globalCtx->kval());

		benchmark_dump_after_kCCF(globalCtx, ctx);
		globalCtx->OutFile << "Nodes count of kCCF: " << kccf.recursiveAllNodesCount() << endl << endl;
	}


	if (globalCtx->options.showkSF) {

		print_title(globalCtx, "DUMP of K-SF");

		dump(*kSFCopy, globalCtx->OutFile);

		delete kSFCopy;
	}	

	if (globalCtx->options.showkSF2) {

		print_title(globalCtx, "DUMP of K-SF with resetted counters");

		dump(ctx->kSlabForest, globalCtx->OutFile);
	}

	globalCtx->OutFile << "\n\n\n";

	if (globalCtx->options.showkCCF) {

		print_title(globalCtx, "DUMP of K-CCF");	
		dump(kccf, globalCtx->OutFile);
	}

}

void printThreadContextInfo(kCCFContextClass *globalCtx, kCCFThreadContextClass *ctx) 
{
	if (globalCtx->WorkingMode() == FuncMode || globalCtx->WorkingMode() == BlockMode) {
	
		printContextInfo(globalCtx, ctx);
		return;
	}

	for (map<ADDRINT, kCCFTradModeContext*>::iterator it = ctx->tradModeContexts.begin(); it != ctx->tradModeContexts.end(); it++)
	{

		kCCFTradModeContext *tradCtx = it->second;
		ADDRINT funcAddr = tradCtx->getFunctionAddr();
		FunctionObj *fc = globalCtx->allFuncs[funcAddr];

		print_func_name(globalCtx, fc->functionName());

		BENCHMARK_SET_FUNC(funcAddr);

		printContextInfo(globalCtx, tradCtx);
	}
}

void blockFuncMode_joinThreads(kCCFContextClass *globalCtx) {

	kCCFThreadContextClass *thCtx = globalCtx->threadContexts[0];
	kCCFThreadContextClass *thCtx2;
	kCCFForest *forest = &thCtx->kSlabForest;

	for (unsigned i=1; i < globalCtx->threadContexts.size(); i++) {
		
		thCtx2 = globalCtx->threadContexts[i];
			
		*forest=forest->join(thCtx2->kSlabForest);

#if KCCF_BENCHMARK
		thCtx->_BM_sumBenchmarkInfo(thCtx2);
#endif

	}

	if (globalCtx->threadContexts.size() > 1)
		globalCtx->threadContexts.erase(globalCtx->threadContexts.begin()+1,globalCtx->threadContexts.end());
}

void tradMode_joinThreads(kCCFContextClass *globalCtx) {

	kCCFThreadContextClass *th0Ctx = globalCtx->threadContexts[0];
	kCCFThreadContextClass *thCtx2;
	kCCFTradModeContext *tradCtx;
	kCCFForest *forest;

	for (FuncsMapIt funcIt = globalCtx->allFuncs.begin(); funcIt != globalCtx->allFuncs.end(); funcIt++)
	{

		//if (!globalCtx->hasToTraceByName(funcIt->second->functionName(), funcIt->second->functionAddress()))
		//	continue;

		if (!globalCtx->hasToTrace(funcIt->second->functionAddress()))
			continue;

		tradCtx = th0Ctx->getFunctionCtx(funcIt->first);
		forest = &tradCtx->kSlabForest;

		for (unsigned i=1; i < globalCtx->threadContexts.size(); i++) 
		{
	
			thCtx2 = globalCtx->threadContexts[i];

			map<ADDRINT, kCCFTradModeContext*>::iterator it = thCtx2->tradModeContexts.find(funcIt->first);

			if (it != thCtx2->tradModeContexts.end()) 
			{
				*forest = forest->join(it->second->kSlabForest);
#if KCCF_BENCHMARK
				tradCtx->_BM_sumBenchmarkInfo(it->second);
#endif
			}
		}

	}

	if (globalCtx->threadContexts.size() > 1)
		globalCtx->threadContexts.erase(globalCtx->threadContexts.begin()+1,globalCtx->threadContexts.end());

}

VOID Fini(INT32 code, VOID *)
{

	//kCCFContextClass *ctx = (kCCFContextClass*)v;
	kCCFContextClass *ctx = globalSharedContext;

	ctx->OutFile << endl << endl << endl;

	ctx->OutFile << "Size of a node: " << sizeof(kCCFNode) << endl;
	
	if (!ctx->options.kinf) {
		ctx->OutFile << "K value: " << ctx->kval() << endl;
	} else {
		ctx->OutFile << "K value: INFINITE" << endl;
	}

	if (ctx->WorkingMode() != FuncMode) {
	
		ctx->OutFile << "Basic blocks count: " << ctx->allBlocks.size() << endl;

	} else {

		ctx->OutFile << "Functions count: " << ctx->allFuncs.size() << endl;

	}

	ctx->OutFile << "Maximum number of different threads: " << ctx->threadContexts.size() << endl;
	ctx->OutFile << endl << endl;

	if (ctx->options.joinThreads) {
	
		if (ctx->WorkingMode() != TradMode)
			blockFuncMode_joinThreads(ctx);
		else
			tradMode_joinThreads(ctx);
	}

	
	for (unsigned i=0; i < ctx->threadContexts.size(); i++) {
	
		if (!ctx->options.joinThreads)
			print_thread_id(ctx, ctx->threadContexts[i]->threadID);

		//Operations in printContextInfo (inversion of the K slab forest and others)
		//have to counted as operations made by thread "threadID" and not by current thread.
		BENCHMARK_SET_THREAD(ctx->threadContexts[i]->threadID);

		//cerr << "Computing kCCF for thread " << ctx->threadContexts[i]->threadID << "...\n";

		printThreadContextInfo(ctx, ctx->threadContexts[i]);
	}

	size_t maxFuncLen=0;

	if (ctx->options.showFuncs || (ctx->options.showBlocks && !ctx->funcsToTrace.size())) {
		
		for (map<ADDRINT,FunctionObj*>::iterator it = ctx->allFuncs.begin(); it != ctx->allFuncs.end(); it++) {
		
			FunctionObj *fc = it->second;

			if (fc->functionName().size() > maxFuncLen)
				maxFuncLen = fc->functionName().size();
		}	
	}

	if (ctx->options.showBlocks) {

		size_t maxLen=0;

		if (ctx->funcsToTrace.size()) {
		
			for (set<string>::iterator it = ctx->funcsToTrace.begin(); it != ctx->funcsToTrace.end(); it++)
				if (it->size() > maxLen)
					maxLen=it->size();

		} else {
		
			maxLen=maxFuncLen;
		}

		ctx->OutFile << "\n\n\n";

		print_title(ctx, "All basic blocks");

		BlocksMapIt it = ctx->allBlocks.begin();
		BlocksMapIt end = ctx->allBlocks.end();

		while (it != end) {
	
			BasicBlock& bb = *it->second;

			ctx->OutFile << "block: ";
			ctx->OutFile.width(maxLen+12);
			ctx->OutFile << left << bb;
			ctx->OutFile << " addr: 0x";
			ctx->OutFile << hex << (void*)bb.getKey() << dec; 
			ctx->OutFile << " simpleCounter: " << bb.getSimpleCounter();

			if (ctx->options.disasm)
			{
				ctx->OutFile << "\tDisassembly: \n";
				for (vector<string>::iterator it2 = bb.instructions.begin(); it2 != bb.instructions.end(); it2++) 
				{
					string ins = *it2;

					if (ins[0] == 'j') {

						size_t space_ch;
						for (space_ch=0; space_ch < ins.size(); space_ch++)
							if (ins[space_ch] == ' ')
								break;
					
						if (space_ch != ins.size()) {
					
						
							string ins_name = ins.substr(0, space_ch);
							string ins_jaddr = ins.substr(space_ch+1);
							ADDRINT addr = strtoul(ins_jaddr.c_str(), 0, 16);
							BlocksMapIt it3 = ctx->allBlocks.find(addr);

							if (it3 != ctx->allBlocks.end())
								ins = ins_name + " " + (string)*it3->second;
						
						}
					}

					ctx->OutFile.width(maxLen+12);
					ctx->OutFile << "\t\t\t\t\t\t\t\t" << ins << endl;
				}

				ctx->OutFile << endl;
			}

			ctx->OutFile << endl;

			it++;
		}
	
	}

	if (ctx->options.showFuncs) {
		
		ctx->OutFile << "\n\n\n";

		print_title(ctx, "All functions");

		for (FuncsMapIt it = ctx->allFuncs.begin(); it != ctx->allFuncs.end(); it++) {
		
			FunctionObj& fc = *it->second;
			
			ctx->OutFile << "function: ";
			ctx->OutFile.width(maxFuncLen+4);
			ctx->OutFile << left << fc;
			ctx->OutFile << " addr: 0x" << hex << (void*)fc.getKey() << dec; 
			ctx->OutFile << " simpleCounter: " << fc.getSimpleCounter() << endl;
		}
	}

    ctx->OutFile.close();

	delete ctx;
}

void funcTraceDebugDump(kCCFContextClass *globalCtx, FunctionObj *fc, 
						kCCFThreadContextClass *ctx, ADDRINT reg_sp, 
						kCCFNode *treeTop, kCCFNode *treeBottom) 
{

	for (unsigned int i=0; i < ctx->shadowStack.size(); i++)
		globalCtx->OutFile << "    ";
	
	if (globalCtx->threadContexts.size() > 1)
		globalCtx->OutFile << "[ Thread: " << PIN_ThreadUid() << " ] " << fc->functionName() << endl;
	else
		globalCtx->OutFile << fc->functionName() << endl;
		
	globalCtx->OutFile << " ----------------------------------------------------------------------------------------------- \n";
}