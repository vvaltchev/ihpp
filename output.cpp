
#include <ctype.h>
#include "config.h"
#include "debug.h"
#include "ihpp.h"

using namespace std;

#include "benchmark.h"
#include "output.h"

#include "tracingFuncs.h"

#define OUTPUT_LINE	"=============================================================================================\n"
#define OUTPUT_LINE2	"=============================================================================================\n\n"

#define OUTPUT_PLINE	"---------------------------------------------------------------------------------------------\n"
#define OUTPUT_PLINE2	"---------------------------------------------------------------------------------------------\n\n"


inline void openTag(const char *tag, bool ret=false, ostream &s = globalSharedContext->OutFile) {

	s << "<" << tag << (ret ? ">\n" : ">");
}

inline void closeTag(const char *tag, ostream &s = globalSharedContext->OutFile) {

	s << "</" << tag << ">\n";
}

inline void openAttr(const char *tag, ostream &s = globalSharedContext->OutFile) {

	s << tag << "=\"";
}

inline void closeAttr(const char *tag, ostream &s = globalSharedContext->OutFile) {

	s << "\" ";
}

inline void benchmark_dump_before_kCCF(ihppContextClass *globalCtx, BenchmarkObj *ctx)
{

#if IHPP_BENCHMARK 

	globalCtx->OutFile << endl;
	globalCtx->OutFile << "Before creation of k-CCF\n";
	globalCtx->OutFile << "Nodes created (valid):  " << ctx->nodes_created << endl;
	globalCtx->OutFile << "Nodes created (empty):  " << ctx->empty_nodes_created << endl;
	globalCtx->OutFile << "Nodes copied:           " << ctx->nodes_copied << endl;
	globalCtx->OutFile << endl;

#endif

}

inline void benchmark_dump_after_kCCF(ihppContextClass *globalCtx, BenchmarkObj *ctx)
{

#if IHPP_BENCHMARK 

	globalCtx->OutFile << "After the creation of k-CCF\n";
	globalCtx->OutFile << "Nodes created (valid):  " << ctx->nodes_created << endl;
	globalCtx->OutFile << "Nodes created (empty):  " << ctx->empty_nodes_created << endl;
	globalCtx->OutFile << "Nodes copied:           " << ctx->nodes_copied << endl;
	globalCtx->OutFile << "Forests copied:         " << ctx->forests_copied << endl << endl;

#endif

}

template <typename T>
inline void print_title(ihppContextClass *globalCtx, T str)
{
	globalCtx->OutFile << OUTPUT_PLINE;
	globalCtx->OutFile << str << "\n";
	globalCtx->OutFile << OUTPUT_PLINE2;
}


template <typename T>
inline void print_thread_id(ihppContextClass *ctx, T str)
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
		
		if (!globalSharedContext->options.joinThreads)
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
inline void print_func_name(ihppContextClass *ctx, T str)
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

void dumpXmlNode(ihppNode &n, int ident=0) {

	ostream &o = globalSharedContext->OutFile;


	openTag("objAddr");
	o << "0x" << hex << (void*)n.getKey() << dec;
	closeTag("objAddr");

	openTag("counter");
	o << n.getCounter();
	closeTag("counter");

	if (n.childrenCount()) {

		openTag("children",true);
	
		ihppNode::nodesIterator it;

		for (it = n.getNodesIteratorBegin(); it != n.getNodesIteratorEnd(); it++) {

			openTag("child",true);
			dumpXmlNode(*it,ident+1);
			closeTag("child");
		}
	
		closeTag("children");
	}

}

void dumpXmlForest(ihppForest &f) {

	ihppForest::treesIterator it;

	for (it = f.getTreesIteratorBegin(); it != f.getTreesIteratorEnd(); it++) {
	
		openTag("tree",true);
		dumpXmlNode(*it);
		closeTag("tree");
	}
}

void printContextInfo(ihppContextClass *globalCtx, ihppAbstractContext *ctx) {

	ihppForest kccf;
	ihppForest *kSFCopy = 0;

	//Operations in this copy shouldn't be counted in benchmark
	//because they are computed only for a visualization propouse:
	//the K slab forest need to copied in order to save it's counter values
	//resetted by kSlabForestKLevelCountersClear() before forest inversion operation
	if (globalCtx->options.showkSF) {
	
		BENCHMARK_OFF

		kSFCopy = new ihppForest;
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

void printThreadContextInfo(ihppContextClass *globalCtx, ihppThreadContextClass *ctx) 
{
	if (globalCtx->WorkingMode() == FuncMode || globalCtx->WorkingMode() == BlockMode) {
	
		printContextInfo(globalCtx, ctx);
		return;
	}

	if (globalCtx->options.xmloutput) {
		
		openTag("intraMode_ctx",true);
	}

	for (map<ADDRINT, ihppIntraModeContext*>::iterator it = ctx->intraModeContexts.begin(); it != ctx->intraModeContexts.end(); it++)
	{

		ihppIntraModeContext *intraCtx = it->second;
		ADDRINT funcAddr = intraCtx->getFunctionAddr();
		FunctionObj *fc = globalCtx->allFuncs[funcAddr];

		if (!globalCtx->options.xmloutput)
			print_func_name(globalCtx, fc->functionName());

		BENCHMARK_SET_FUNC(funcAddr);

		if (globalCtx->options.xmloutput) {

			openTag("func_ctx",true);

			openTag("funcAddr");
			globalCtx->OutFile << "0x" << hex << (void*)fc->functionAddress() << dec;
			closeTag("funcAddr");
		}

		printContextInfo(globalCtx, intraCtx);

		if (globalCtx->options.xmloutput)
			closeTag("func_ctx");
	}

	if (globalCtx->options.xmloutput)
		closeTag("intraMode_ctx");

}

void blockFuncMode_joinThreads(ihppContextClass *globalCtx) {

	ihppThreadContextClass *thCtx = globalCtx->threadContexts[0];
	ihppThreadContextClass *thCtx2;
	ihppForest *forest = &thCtx->kSlabForest;

	for (unsigned i=1; i < globalCtx->threadContexts.size(); i++) {
		
		thCtx2 = globalCtx->threadContexts[i];
			
		*forest=forest->join(thCtx2->kSlabForest);

#if IHPP_BENCHMARK
		thCtx->_BM_sumBenchmarkInfo(thCtx2);
#endif

	}

	if (globalCtx->threadContexts.size() > 1)
		globalCtx->threadContexts.erase(globalCtx->threadContexts.begin()+1,globalCtx->threadContexts.end());
}

void intraMode_joinThreads(ihppContextClass *globalCtx) {

	ihppThreadContextClass *th0Ctx = globalCtx->threadContexts[0];
	ihppThreadContextClass *thCtx2;
	ihppIntraModeContext *intraCtx;
	ihppForest *forest;

	for (FuncsMapIt funcIt = globalCtx->allFuncs.begin(); funcIt != globalCtx->allFuncs.end(); funcIt++)
	{

		if (!globalCtx->hasToTrace(funcIt->second->functionAddress()))
			continue;

		intraCtx = th0Ctx->getFunctionCtx(funcIt->first);
		forest = &intraCtx->kSlabForest;

		for (unsigned i=1; i < globalCtx->threadContexts.size(); i++) 
		{
	
			thCtx2 = globalCtx->threadContexts[i];

			map<ADDRINT, ihppIntraModeContext*>::iterator it = thCtx2->intraModeContexts.find(funcIt->first);

			if (it != thCtx2->intraModeContexts.end()) 
			{
				*forest = forest->join(it->second->kSlabForest);
#if IHPP_BENCHMARK
				intraCtx->_BM_sumBenchmarkInfo(it->second);
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

	ihppContextClass *globalCtx = globalSharedContext;

	string ins = insData.ins_text;

	string ins_name = getInsName(ins);

	if (!ins_name.size())
		return ins;

	ADDRINT addr = insData.targetAddr;

	if (insData.externFuncName) {

		ADDRINT diff = insData.targetAddr - insData.targetFuncAddr;
		string diffStr;

		if (diff > 0)
			diffStr = string()+diff;

		ins = ins_name + string(" ") + string(insData.externFuncName) + string("+") + diffStr;

	} else {

		FuncsMapIt it3 = globalCtx->allFuncs.find(addr);

		if (insData.isCall) {

			if (it3 != globalCtx->allFuncs.end()) {

				ins = ins_name + string(" ") + it3->second->functionName();
			
			} else {

				FuncsMapIt it3 = globalCtx->allFuncs.find(insData.targetFuncAddr);

				if (it3 == globalCtx->allFuncs.end())
					return ins;

				ADDRINT diff = addr - insData.targetFuncAddr;

				ins = ins_name + string(" ") + it3->second->functionName() + string("+") + diff;

				insInfo targetIns = it3->second->instructions[addr];
				
				if (strlen(targetIns.ins_text) && targetIns.isDirectBranchOrCall())
					ins += string(" --> ") + makeHumanJump(targetIns);

			}


		} else {
		
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

		}
	}



	return ins;
}

void makeHumanDisasm() {

	ihppContextClass *globalCtx = globalSharedContext;

	for (FuncsMapIt funcIt = globalCtx->allFuncs.begin(); funcIt != globalCtx->allFuncs.end(); funcIt++)
	{
		map<ADDRINT,insInfo>::iterator it;

		for (it = (funcIt->second)->instructions.begin(); it != (funcIt->second)->instructions.end(); it++)
		{
			string ins = it->second.ins_text;
			
			if (!it->second.isDirectBranchOrCall())
				continue;

			ins = makeHumanJump(it->second);

			//it->second.ins_text=ins;
			delete[] it->second.ins_text;
			it->second.ins_text=duplicate_string(ins);
		}
	}
}




void writeXmlConfig() {

	ihppContextClass *ctx = globalSharedContext;
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

	ihppContextClass *ctx = globalSharedContext;

	if (!ctx->options.xmloutput) {

		if (ctx->options.blocksDisasm || ctx->options.funcsDisasm)
			makeHumanDisasm();

		ctx->OutFile << endl << endl << endl;

		ctx->OutFile << "Size of a node: " << sizeof(ihppNode) << endl;
	
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

	ihppContextClass *ctx = globalSharedContext;
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


void print_ins(ADDRINT addr, insInfo &info) {

	ihppContextClass *ctx = globalSharedContext;

	openAttr("address");
	ctx->OutFile << "0x" << hex << (void*)addr << dec;
	closeAttr("address");

	openAttr("isDirectBranch");
	ctx->OutFile << (info.isDirectBranchOrCall() && !info.isCall);
	closeAttr("isDirectBranch");

	openAttr("isDirectCall");
	ctx->OutFile << (info.isDirectBranchOrCall() && info.isCall);
	closeAttr("isDirectCall");

	if (info.targetAddr) {
		openAttr("targetAddr");
		ctx->OutFile << "0x" << hex << (void*)info.targetAddr << dec;
		closeAttr("targetAddr");
	}

	if (info.targetFuncAddr) {
		openAttr("targetAddrFunc");
		ctx->OutFile << "0x" << hex << (void*)info.targetFuncAddr << dec;
		closeAttr("targetAddrFunc");
	}

	if (isStringPrintable(info.externFuncName)) {
						
		openAttr("externFuncName");
		ctx->OutFile << info.externFuncName;
		closeAttr("externFuncName");
	}

	ctx->OutFile << ">\n";

	openTag("disasm");
	ctx->OutFile << info.ins_text;
	closeTag("disasm");

	if (info.externFuncName)
		return;

	if (!info.isCall)
		return;

	FuncsMapIt it = ctx->allFuncs.find(addr);

	if (it != ctx->allFuncs.end())
		return;

	FuncsMapIt it2 = ctx->allFuncs.find(info.targetFuncAddr);

	if (it2 == ctx->allFuncs.end())
		return;

	insInfo targetIns = it2->second->instructions[info.targetAddr];

	if (strlen(targetIns.ins_text) && targetIns.isDirectBranchOrCall()) {

		openTag("twoStepCallIns", true);
		print_ins(info.targetAddr, targetIns);
		closeTag("twoStepCallIns");
	}
}


void print_showBlocks(size_t maxFuncLen) {

	ihppContextClass *ctx = globalSharedContext;
	size_t maxLen=0;

	if (!ctx->options.xmloutput) {

		if (ctx->funcsToTrace.size()) {
		
			for (set<string>::iterator it = ctx->funcsToTrace.begin(); it != ctx->funcsToTrace.end(); it++)
				if (it->size() > maxLen)
					maxLen=it->size();

		} else {
		
			maxLen=maxFuncLen;
		}


		ctx->OutFile << "\n\n\n";
		print_title(ctx, "All basic blocks");
	
	} else {
	
		openTag("basicblocks",true);

	}


	for(BlocksMapIt it = ctx->allBlocks.begin(); it != ctx->allBlocks.end(); it++) {
	
		BasicBlock& bb = *it->second;

		if (!ctx->options.xmloutput) {

			ctx->OutFile << "block: ";
			ctx->OutFile.width(maxLen+12);
			ctx->OutFile << left << bb;
			ctx->OutFile << " addr: 0x";
			ctx->OutFile << hex << (void*)bb.getKey() << dec; 
			ctx->OutFile << " simpleCounter: " << bb.getSimpleCounter();

		} else {
		
			ctx->OutFile << "<bb ";

			openAttr("firstInsAddr");
			ctx->OutFile << "0x" << hex << (void*)bb.blockAddress() << dec;
			closeAttr("firstInsAddr");

			openAttr("lastInsAddr");
			ctx->OutFile << "0x" << hex << (void*)bb.blockEndAddress() << dec;
			closeAttr("lastInsAddr");

			openAttr("funcAddr");
			ctx->OutFile << "0x" << hex << (void*)bb.functionAddr() << dec;
			closeAttr("funcAddr");

			openAttr("line");
			ctx->OutFile << bb.firstLine();
			closeAttr("line");

			openAttr("col");
			ctx->OutFile << bb.firstCh();
			closeAttr("col");	
		}

		if (ctx->options.blocksDisasm)
		{
			if (!ctx->options.xmloutput) {
				ctx->OutFile << "\tDisassembly: \n";
			
			} else {
				ctx->OutFile << ">\n";
				openTag("instructions",true);
			}
			
			map<ADDRINT,insInfo>::iterator insIt;

			for (insIt = bb.functionPtr->instructions.begin(); insIt != bb.functionPtr->instructions.end(); insIt++)
				if (insIt->first == bb.blockAddress())
					break;

			for (; insIt != bb.functionPtr->instructions.end(); insIt++)
			{
				if (insIt->first > bb.blockEndAddress())
					break;

				if (!ctx->options.xmloutput) {
					string ins = insIt->second.ins_text;
					ctx->OutFile.width(maxLen+12);
					ctx->OutFile << "\t\t\t\t\t\t\t\t" << ins << endl;
				} else {
				
					ctx->OutFile << "<ins ";

					print_ins(insIt->first, insIt->second);

					closeTag("ins");
				}
			}


			if (!ctx->options.xmloutput) {
				ctx->OutFile << endl;
			} else {
				closeTag("instructions");
				closeTag("bb");
			}

		} else {
		
			if (ctx->options.xmloutput)
				ctx->OutFile << "/>" << endl;
		}


		if (!ctx->options.blocksDisasm)
			ctx->OutFile << endl;

	}

	if (ctx->options.xmloutput)
		closeTag("basicblocks");
}

void print_showFuncs(size_t maxFuncLen) {

	ihppContextClass *ctx = globalSharedContext;

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

			if (ctx->options.funcsDisasm) {

				map<ADDRINT,insInfo>::iterator insIt;
			
				ctx->OutFile << endl;

				for (insIt = fc.instructions.begin(); insIt != fc.instructions.end(); insIt++) {
			
					ctx->OutFile << "\t\t";
					ctx->OutFile << hex << (void*)insIt->first << dec << "    "; 
					ctx->OutFile << insIt->second.ins_text << endl;
				}

				ctx->OutFile << endl;

			}

		} else {
				
			openTag("func",true);
				
			openTag("name");
			ctx->OutFile << fc.functionName();
			closeTag("name");

			openTag("address");
			ctx->OutFile << "0x" << hex << (void*)fc.functionAddress() << dec;
			closeTag("address");

			openTag("simpleCounter");
			ctx->OutFile << "0x" << fc.getSimpleCounter();
			closeTag("simpleCounter");


			if (fc.fileName().size()) {
				
				openTag("fileName");
				ctx->OutFile << fc.fileName();
				closeTag("fileName");
			}

			if (fc.instructions.size() && ctx->options.funcsDisasm) {
					
				openTag("instructions",true);

				map<ADDRINT,insInfo>::iterator insIt;

				for (insIt = fc.instructions.begin(); insIt != fc.instructions.end(); insIt++) 
				{

					ctx->OutFile << "<ins ";

					print_ins(insIt->first, insIt->second);

					closeTag("ins");
				}

				closeTag("instructions");
			}

			closeTag("func");
		}
	}

	if (ctx->options.xmloutput)
		closeTag("functions");
}

void freeMemory() {

	ihppContextClass *ctx = globalSharedContext;

	for (FuncsMapIt it = ctx->allFuncs.begin(); it != ctx->allFuncs.end(); it++) {
		
		FunctionObj& fc = *it->second;
		
		map<ADDRINT,insInfo>::iterator insIt;
			
		for (insIt = fc.instructions.begin(); insIt != fc.instructions.end(); insIt++) {
		
			delete [] insIt->second.ins_text;

			if (insIt->second.externFuncName)
				delete [] insIt->second.externFuncName;
		}
		
	}

}

void Fini(INT32 code, void *)
{
	ihppContextClass *ctx = globalSharedContext;


	print_outputInit();


	if (ctx->options.joinThreads) {
	
		if (ctx->WorkingMode() != IntraMode)
			blockFuncMode_joinThreads(ctx);
		else
			intraMode_joinThreads(ctx);
	}

	if (ctx->options.xmloutput)
		openTag("threads",true);
	
	for (unsigned i=0; i < ctx->threadContexts.size(); i++) {
	
		print_openThread(ctx->threadContexts[i]->threadID);



		//Operations in printContextInfo (inversion of the K slab forest and others)
		//have to counted as operations made by thread "threadID" and not by current thread.
		BENCHMARK_SET_THREAD(ctx->threadContexts[i]->threadID);

		//cerr << "Computing kCCF for thread " << ctx->threadContexts[i]->threadID << "...\n";

		printThreadContextInfo(ctx, ctx->threadContexts[i]);

		print_closeThread();
	}

	if (ctx->options.xmloutput)
		closeTag("threads");

	size_t maxFuncLen=getMaxFuncLen();

	if (ctx->options.showBlocks) {

		print_showBlocks(maxFuncLen);
	}

	if (ctx->options.showFuncs) {
		
		print_showFuncs(maxFuncLen);
	}

	if (ctx->options.xmloutput)
		closeTag("output");

	/* 
		********************************
		Finalization
		********************************
	*/

	ctx->OutFile.close();

	freeMemory();
	delete ctx;
}

void funcTraceDebugDump(ihppContextClass *globalCtx, FunctionObj *fc, 
						ihppThreadContextClass *ctx, ADDRINT reg_sp, 
						ihppNode *treeTop, ihppNode *treeBottom) 
{

	for (unsigned int i=0; i < ctx->shadowStack.size(); i++)
		globalCtx->OutFile << "    ";
	
	if (globalCtx->threadContexts.size() > 1)
		globalCtx->OutFile << "[ Thread: " << PIN_ThreadUid() << " ] " << fc->functionName() << endl;
	else
		globalCtx->OutFile << fc->functionName() << endl;
		
	globalCtx->OutFile << OUTPUT_PLINE;
}