
#include <ctype.h>
#include "config.h"
#include "debug.h"
#include "kCCF.h"

using namespace std;

#include "benchmark.h"
#include "output.h"

#include "tracingFuncs.h"

#define OUTPUT_LINE		"=============================================================================================\n"
#define OUTPUT_LINE2	"=============================================================================================\n\n"

#define OUTPUT_PLINE	"---------------------------------------------------------------------------------------------\n"
#define OUTPUT_PLINE2	"---------------------------------------------------------------------------------------------\n\n"


inline void openTag(const char *tag, bool ret=false, ostream &s = globalSharedContext->OutFile) {

	s << "<" << tag << (ret ? ">\n" : ">");
}

inline void closeTag(const char *tag, ostream &s = globalSharedContext->OutFile) {

	s << "</" << tag << ">\n";
}

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
inline void print_openThread(T th)
{
	if (globalSharedContext->options.xmloutput) {
	
		openTag("thread", true);

		openTag("id");
		globalSharedContext->OutFile << th;
		closeTag("id");

	} else {

		if (!globalSharedContext->options.joinThreads)
			print_thread_id(globalSharedContext, th);
	}
}


inline void print_closeThread()
{
	if (globalSharedContext->options.xmloutput)
		closeTag("thread");
}


template <typename T>
inline void print_func_name(kCCFContextClass *ctx, T str)
{
	ctx->OutFile << OUTPUT_LINE;
	ctx->OutFile << "Function: " << str << "()\n";
	ctx->OutFile << OUTPUT_LINE;
}

bool isStringPrintable(const char *str) {

	const char *ptr = str;

	if (!str)
		return false;

	while (*ptr)
		if (!isprint(*ptr++))
			return false;

	return true;
}

void dumpXmlNode(kCCFNode &n, int ident=0) {

	kCCFContextClass *ctx = globalSharedContext;
	ostream &o = globalSharedContext->OutFile;


	openTag("objAddr");
	o << "0x" << hex << (void*)n.getKey() << dec;
	closeTag("objAddr");

	openTag("counter");
	o << n.getCounter();
	closeTag("counter");

	if (n.childrenCount()) {

		openTag("children",true);
	
		kCCFNode::nodesIterator it;

		for (it = n.getNodesIteratorBegin(); it != n.getNodesIteratorEnd(); it++) {

			openTag("child",true);
			dumpXmlNode(*it,ident+1);
			closeTag("child");
		}
	
		closeTag("children");
	}

}

void dumpXmlForest(kCCFForest &f) {

	kCCFContextClass *ctx = globalSharedContext;
	ostream &o = globalSharedContext->OutFile;

	kCCFForest::treesIterator it;

	for (it = f.getTreesIteratorBegin(); it != f.getTreesIteratorEnd(); it++) {
	
		openTag("tree",true);
		dumpXmlNode(*it);
		closeTag("tree");
	}
}

void printContextInfo(kCCFContextClass *globalCtx, kCCFAbstractContext *ctx) {

	kCCFForest kccf;
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

	if (!globalCtx->options.xmloutput) {
		
		globalCtx->OutFile << "Nodes count of k slab forest: " << ctx->kSlabForest.recursiveAllNodesCount() << endl;
		benchmark_dump_before_kCCF(globalCtx, ctx);
	}

	

	if (globalCtx->options.showkCCF) {
	
		kSlabForestKLevelCountersClear(ctx->kSlabForest, ctx->rootKey, globalCtx->kval());
		kccf = ctx->kSlabForest.inverseK(globalCtx->kval());

		if (!globalCtx->options.xmloutput) {
			
			benchmark_dump_after_kCCF(globalCtx, ctx);
			globalCtx->OutFile << "Nodes count of kCCF: " << kccf.recursiveAllNodesCount() << endl << endl;
		}
	}


	if (globalCtx->options.showkSF) {

		if (!globalCtx->options.xmloutput) {

			print_title(globalCtx, "DUMP of K-SF");
			dump(*kSFCopy, globalCtx->OutFile);

		} else {
			
			openTag("kSF",true);
			dumpXmlForest(*kSFCopy);
			closeTag("kSF");
		}

		delete kSFCopy;
	}	

	if (globalCtx->options.showkSF2) {

		if (!globalCtx->options.xmloutput) {
			print_title(globalCtx, "DUMP of K-SF with resetted counters");
			dump(ctx->kSlabForest, globalCtx->OutFile);
		}
	}

	if (!globalCtx->options.xmloutput)
		globalCtx->OutFile << "\n\n\n";

	if (globalCtx->options.showkCCF) {

		if (!globalCtx->options.xmloutput) {

			print_title(globalCtx, "DUMP of K-CCF");	
			dump(kccf, globalCtx->OutFile);

		} else {
		
			openTag("kCCF",true);
			dumpXmlForest(kccf);
			closeTag("kCCF");
		}
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

		if (!globalCtx->options.xmloutput)
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

string getInsName(string ins) {

	size_t space_ch;
	for (space_ch=0; space_ch < ins.size(); space_ch++)
		if (ins[space_ch] == ' ')
			break;
					
	if (space_ch == ins.size())
		return string();
				
	return ins.substr(0, space_ch);
}

string makeHumanJump(insInfo &insData) {

	kCCFContextClass *globalCtx = globalSharedContext;

	string ins = insData.ins_text;

	string ins_name = getInsName(ins);

	if (!ins_name.size())
		return ins;

	ADDRINT addr = insData.targetAddr;

	//cerr << "ins: " << ins << ", ins_name = " << ins_name << endl;	


	if (insData.externFuncName) {
		
		//cerr << "extern func\n";

		ADDRINT diff = insData.targetAddr - insData.targetFuncAddr;
		string diffStr;

		if (diff > 0)
			diffStr = string()+diff;

		ins = ins_name + string(" ") + string(insData.externFuncName) + diffStr;

	} else {

		//cerr << "local func, target func addr: " << hex << (void*) insData.targetFuncAddr << dec << endl;

		FuncsMapIt it3 = globalCtx->allFuncs.find(addr);

		if (insData.isCall) {

			//cerr << "is call..\n";

			if (it3 != globalCtx->allFuncs.end()) {

				//cerr << "found function\n";

				ins = ins_name + string(" ") + it3->second->functionName();
			
			} else {
			
				//cerr << "looking for targetFuncAddr..\n";

				FuncsMapIt it3 = globalCtx->allFuncs.find(insData.targetFuncAddr);

				if (it3 == globalCtx->allFuncs.end())
					return ins;

				ADDRINT diff = addr - insData.targetFuncAddr;

				ins = ins_name + string(" ") + it3->second->functionName() + string("+") + diff;

				insInfo targetIns = it3->second->instructions[addr];

				//cerr << "targetIns text: " << targetIns.ins_text << endl;
				
				if (targetIns.ins_text.size() && targetIns.isDirectBranchOrCall())
					ins += string(" --> ") + makeHumanJump(targetIns);
			}

			//cerr << "new ins: " << ins << endl;

		} else {
		
			//cerr << "is branch..\n";

			BlocksMapIt it4 = globalCtx->allBlocks.find(addr);

			if (it4 != globalCtx->allBlocks.end()) {
				
				ins = ins_name + " " + (string)*it4->second;
				return ins;
			}

			
			if (it3 != globalCtx->allFuncs.end()) {
				
				ins = ins_name + string(" ") + it3->second->functionName();
				return ins;
			}

			it3 = globalCtx->allFuncs.find(insData.targetFuncAddr);
			ADDRINT diff = insData.targetAddr - insData.targetFuncAddr;

			if (it3 != globalCtx->allFuncs.end())
				ins = ins_name + string(" ") + it3->second->functionName() + string("+") + diff;

			//cerr << "new ins: " << ins << endl;
		}
	}



	return ins;
}

void makeHumanDisasm() {

	kCCFContextClass *globalCtx = globalSharedContext;

	for (FuncsMapIt funcIt = globalCtx->allFuncs.begin(); funcIt != globalCtx->allFuncs.end(); funcIt++)
	{
		map<ADDRINT,insInfo>::iterator it;

		//if ( FUNC_IS_TEXT_N(funcIt->second->functionName()) )
		//	continue;

		for (it = (funcIt->second)->instructions.begin(); it != (funcIt->second)->instructions.end(); it++)
		{
			string ins = it->second.ins_text;
			
			if (!it->second.isDirectBranchOrCall())
				continue;

			ins = makeHumanJump(it->second);

			it->second.ins_text=ins;
			
		}
	}
}




void writeXmlConfig() {

	kCCFContextClass *ctx = globalSharedContext;
	ostream &o = globalSharedContext->OutFile;

	openTag("configuration",true);

	openTag("WorkingMode");
	o << WorkingModesString[ctx->WorkingMode()];
	closeTag("WorkingMode");

	openTag("joinThreads");
	o << ctx->options.joinThreads;
	closeTag("joinThreads");

	openTag("rollLoops");
	o << ctx->options.rollLoops;
	closeTag("rollLoops");

	openTag("kvalue");
	o << ctx->kval();
	closeTag("kvalue");

	openTag("insTracing");
	o << ENABLE_INS_TRACING;
	closeTag("insTracing");

	openTag("subCallCheck");
	o << ENABLE_SUBCALL_CHECK;
	closeTag("subCallCheck");

	openTag("relyOnSpCheck");
	o << ENABLE_RELY_ON_SP_CHECK;
	closeTag("relyOnSpCheck");

	openTag("insTracingFwdJmpRec");
	o << ENABLE_INS_FORWARD_JMP_RECOGNITION;
	closeTag("insTracingFwdJmpRec");

	openTag("windows");
	o << USING_WINDOWS;
	closeTag("windows");

	openTag("archbits");
	o << sizeof(void*)*8;
	closeTag("archbits");

	openTag("winFullTracingLongjmpTracing");
	o << ENABLE_WIN32_FULLTRACE_TARGET_TRACING_LONGJMP;
	closeTag("winFullTracingLongjmpTracing");

	openTag("winMainAlignment");
	o << ENABLE_WIN32_MAIN_ALIGNMENT;
	closeTag("winMainAlignment");

	closeTag("configuration");
}

void print_outputInit() {

	kCCFContextClass *ctx = globalSharedContext;

	if (!ctx->options.xmloutput) {

		if (ctx->options.disasm)
			makeHumanDisasm();

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
	
	} else {
	
		ctx->OutFile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl;
		openTag("output",true);
		writeXmlConfig();
	}

}

size_t getMaxFuncLen() {

	kCCFContextClass *ctx = globalSharedContext;
	size_t maxFuncLen=0;

	if (ctx->options.showFuncs || (ctx->options.showBlocks && !ctx->funcsToTrace.size())) {
		
		for (map<ADDRINT,FunctionObj*>::iterator it = ctx->allFuncs.begin(); it != ctx->allFuncs.end(); it++) {
		
			FunctionObj *fc = it->second;

			if (fc->functionName().size() > maxFuncLen)
				maxFuncLen = fc->functionName().size();
		}	
	}

	return maxFuncLen;
}

void print_showBlocks(size_t maxFuncLen) {

	kCCFContextClass *ctx = globalSharedContext;
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

			map<ADDRINT,insInfo>::iterator it;

			for (it = bb.functionPtr->instructions.begin(); it != bb.functionPtr->instructions.end(); it++)
			{
				if (it->first == bb.blockAddress())
					break;
			}

			for (; it != bb.functionPtr->instructions.end(); it++)
			{
				if (it->first > bb.blockEndAddress())
					break;

				string ins = it->second.ins_text;

				ctx->OutFile.width(maxLen+12);
				ctx->OutFile << "\t\t\t\t\t\t\t\t" << ins << endl;
			}

			ctx->OutFile << endl;
		}

		ctx->OutFile << endl;

		it++;
	}
}

void print_showFuncs(size_t maxFuncLen) {

	kCCFContextClass *ctx = globalSharedContext;

	if (!ctx->options.xmloutput) {

		ctx->OutFile << "\n\n\n";
		print_title(ctx, "All functions");

	} else {
		
		openTag("functions",true);
	}

	for (FuncsMapIt it = ctx->allFuncs.begin(); it != ctx->allFuncs.end(); it++) {
		
		FunctionObj& fc = *it->second;
			
		if (!ctx->options.xmloutput) {
				
			ctx->OutFile << "function: ";
			ctx->OutFile.width(maxFuncLen+4);
			ctx->OutFile << left << fc;
			ctx->OutFile << " addr: 0x" << hex << (void*)fc.getKey() << dec; 
			ctx->OutFile << " simpleCounter: " << fc.getSimpleCounter() << endl;
			
		} else {
				
			openTag("func",true);
				
			openTag("name");
			ctx->OutFile << fc.functionName() << endl;
			closeTag("name");

			openTag("address");
			ctx->OutFile << "0x" << hex << (void*)fc.functionAddress() << dec << endl;
			closeTag("address");

			openTag("fileName");
			ctx->OutFile << fc.fileName() << endl;
			closeTag("fileName");

			if (fc.instructions.size()) {
					
				openTag("instructions");

				map<ADDRINT,insInfo>::iterator it;

				for (it = fc.instructions.begin(); it != fc.instructions.end(); it++) 
				{
					openTag("ins");

					openTag("address");
					ctx->OutFile << "0x" << hex << (void*)it->first << dec << endl;
					closeTag("address");

					openTag("isDirectBranch");
					ctx->OutFile << (it->second.isDirectBranchOrCall() && !it->second.isCall);
					closeTag("isDirectBranch");

					openTag("isDirectCall");
					ctx->OutFile << (it->second.isDirectBranchOrCall() && it->second.isCall);
					closeTag("isDirectCall");

					if (it->second.targetAddr) {
						openTag("targetAddr");
						ctx->OutFile << "0x" << hex << (void*)it->second.targetAddr << dec;
						closeTag("targetAddr");
					}

					if (it->second.targetFuncAddr) {
						openTag("targetAddrFunc");
						ctx->OutFile << "0x" << hex << (void*)it->second.targetFuncAddr << dec;
						closeTag("targetAddrFunc");
					}

					if (isStringPrintable(it->second.externFuncName)) {
						
						openTag("externFuncName");
						ctx->OutFile << it->second.externFuncName;
						closeTag("externFuncName");
					}

					openTag("disasm");
					ctx->OutFile << it->second.ins_text;
					closeTag("disasm");


					closeTag("ins");
				}

				closeTag("instructions");
			}

			closeTag("func");
		}
	}

	closeTag("functions");
}

void Fini(INT32 code, void *)
{
	kCCFContextClass *ctx = globalSharedContext;


	print_outputInit();


	if (ctx->options.joinThreads) {
	
		if (ctx->WorkingMode() != TradMode)
			blockFuncMode_joinThreads(ctx);
		else
			tradMode_joinThreads(ctx);
	}

	
	
	for (unsigned i=0; i < ctx->threadContexts.size(); i++) {
	
		print_openThread(ctx->threadContexts[i]->threadID);



		//Operations in printContextInfo (inversion of the K slab forest and others)
		//have to counted as operations made by thread "threadID" and not by current thread.
		BENCHMARK_SET_THREAD(ctx->threadContexts[i]->threadID);

		//cerr << "Computing kCCF for thread " << ctx->threadContexts[i]->threadID << "...\n";

		printThreadContextInfo(ctx, ctx->threadContexts[i]);

		print_closeThread();
	}

	size_t maxFuncLen=getMaxFuncLen();

	if (ctx->options.showBlocks) {

		print_showBlocks(maxFuncLen);
	}

	if (ctx->options.showFuncs) {
		
		print_showFuncs(maxFuncLen);
	}

	if (ctx->options.xmloutput)
		closeTag("output");

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
		
	globalCtx->OutFile << OUTPUT_PLINE;
}