
#include "specialfuncs.h"
#include "dataStructures.h"

#ifndef __THREAD_CTX_HEADER__
#define __THREAD_CTX_HEADER__

void funcMode_ret();


class ShadowStackType {

public:

	kCCFNode *treeTop;
	kCCFNode *treeBottom;
	ADDRINT stackPtr;

#if ENABLE_INS_FORWARD_JMP_RECOGNITION

	unsigned int fjmps;

#endif

	ShadowStackType(kCCFNode *t, kCCFNode *b, ADDRINT ptr) : treeTop(t), treeBottom(b), stackPtr(ptr) 
	{ 

#if ENABLE_INS_FORWARD_JMP_RECOGNITION

		fjmps=0;
#endif

	}
};

class kCCFAbstractContext: public BenchmarkObj {

public:

	ADDRINT rootKey;
	kCCFForest kSlabForest;
	kCCFNodeMap R;
	
	unsigned int counter;

	kStack<ShadowStackType> shadowStack;

	kCCFAbstractContext() : rootKey(0), counter(0) 
	{ 
		BENCHMARK_INIT_VARS
	}

};

class kCCFTradModeContext : public kCCFAbstractContext {

	ADDRINT funcAddr;

public:

	kCCFTradModeContext(ADDRINT functionAddr) : kCCFAbstractContext(), funcAddr(functionAddr) { }

	ADDRINT getFunctionAddr() { return funcAddr; }
};



class kCCFThreadContextClass : public kCCFAbstractContext {

	ADDRINT currentFunction;

public:
	
	PIN_THREAD_UID threadID;
	
	//BlockMode and FuncMode properies

	kCCFNode *treeTop;
	kCCFNode *treeBottom;

	//FuncMode properties

#if ENABLE_INS_TRACING
	ADDRINT jumpTargetFuncAddr;
	ADDRINT lastJumpTargetFuncAddr;
	bool haveToJump;
#endif
	
#if ENABLE_SUBCALL_CHECK
	UINT32 subcallcount;
#endif

	
#if defined(_WIN32) && ENABLE_WIN32_MAIN_ALIGNMENT
	size_t __tmainCRTStartup_stack_size; 
#endif
	
#if ENABLE_INS_FORWARD_JMP_RECOGNITION
	bool forwardJmpHappened;
	unsigned int lastfjmps;
	ADDRINT fjmpsFuncAddr;
#endif

	bool haveToTrace;

	ADDRINT startFuncAddr;
	ADDRINT stopFuncAddr;

	//TradMode properties
	map<ADDRINT, kCCFTradModeContext*> tradModeContexts;

	//Methods

	kCCFThreadContextClass(PIN_THREAD_UID tid, ADDRINT startFuncAddr, ADDRINT stopFuncAddr);

	~kCCFThreadContextClass();

	inline bool canPopStack();
	inline bool popShadowStack();

	kCCFTradModeContext *getFunctionCtx(ADDRINT funcAddr);
	kCCFTradModeContext *getCurrentFunctionCtx();

	ADDRINT getCurrentFunction();
	kCCFTradModeContext *setCurrentFunction(ADDRINT currFunc);
	string getCurrentFunctionName(); 
};

inline bool kCCFThreadContextClass::canPopStack() {
	
#if defined(_WIN32) && ENABLE_WIN32_MAIN_ALIGNMENT
	
	return shadowStack.size() > 0 && 
			(__tmainCRTStartup_stack_size == (unsigned)-1 || shadowStack.size()-1 >= __tmainCRTStartup_stack_size+1);

#else

	return shadowStack.size() > 0;

#endif
}

inline bool kCCFThreadContextClass::popShadowStack() 
{
	if (canPopStack()) {
			
		funcMode_ret();
		return true;
	} 
		
	return false;
}

inline kCCFThreadContextClass::kCCFThreadContextClass(PIN_THREAD_UID tid, ADDRINT startFuncAddr, ADDRINT stopFuncAddr)
	 :  kCCFAbstractContext(),  currentFunction(0), threadID(tid)
	{
		INIT_SUBCALL_CHECK_VARS();
		INIT_THREAD_CTX_W32_VARS();

#if ENABLE_INS_TRACING
		jumpTargetFuncAddr=0;
		lastJumpTargetFuncAddr=0; 
		haveToJump=false; 
#endif

#if ENABLE_INS_FORWARD_JMP_RECOGNITION
		forwardJmpHappened=false;
		lastfjmps=0;
		fjmpsFuncAddr=0;
#endif

		kCCFThreadContextClass::startFuncAddr = startFuncAddr;
		kCCFThreadContextClass::stopFuncAddr = stopFuncAddr;

		if (startFuncAddr)
			haveToTrace = false;
		else
			haveToTrace = true;
	}


inline kCCFTradModeContext *kCCFThreadContextClass::getCurrentFunctionCtx() 
{ 
	return getFunctionCtx(currentFunction); 
}


inline ADDRINT kCCFThreadContextClass::getCurrentFunction() 
{ 
	return currentFunction; 
}

inline kCCFTradModeContext *kCCFThreadContextClass::setCurrentFunction(ADDRINT currFunc)
{
	currentFunction=currFunc; 
	return getCurrentFunctionCtx();
}

inline kCCFThreadContextClass::~kCCFThreadContextClass()
{
	for (map<ADDRINT, kCCFTradModeContext*>::iterator it = tradModeContexts.begin(); it != tradModeContexts.end(); it++)
		delete it->second;
}

inline kCCFTradModeContext *kCCFThreadContextClass::getFunctionCtx(ADDRINT funcAddr) 
{

	if (tradModeContexts.find(funcAddr) == tradModeContexts.end()) {
	
		kCCFTradModeContext *tradCtx;
		tradCtx = new kCCFTradModeContext(funcAddr);
		tradModeContexts[funcAddr] = tradCtx;
		return tradCtx;
	}

	return tradModeContexts[funcAddr];
}



#endif