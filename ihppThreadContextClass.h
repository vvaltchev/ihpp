
#include "specialfuncs.h"
#include "dataStructures.h"

#ifndef __THREAD_CTX_HEADER__
#define __THREAD_CTX_HEADER__

void funcMode_ret();


class ShadowStackType {

public:

	ihppNode *treeTop;
	ihppNode *treeBottom;
	ADDRINT stackPtr;

#if ENABLE_INS_FORWARD_JMP_RECOGNITION

	unsigned int fjmps;

#endif

	ShadowStackType(ihppNode *t, ihppNode *b, ADDRINT ptr) : treeTop(t), treeBottom(b), stackPtr(ptr) 
	{ 

#if ENABLE_INS_FORWARD_JMP_RECOGNITION

		fjmps=0;
#endif

	}
};

class ihppAbstractContext: public BenchmarkObj {

public:

	ADDRINT rootKey;
	ihppForest kSlabForest;
	ihppNodeMap R;
	
	unsigned int counter;

	ihppStack<ShadowStackType> shadowStack;

	ihppAbstractContext() : rootKey(0), counter(0) 
	{ 
		BENCHMARK_INIT_VARS
	}

};

class ihppIntraModeContext : public ihppAbstractContext {

	ADDRINT funcAddr;

public:

	ihppIntraModeContext(ADDRINT functionAddr) : ihppAbstractContext(), funcAddr(functionAddr) { }

	ADDRINT getFunctionAddr() { return funcAddr; }
};



class ihppThreadContextClass : public ihppAbstractContext {

	ADDRINT currentFunction;

public:
	
	PIN_THREAD_UID threadID;
	
	//BlockMode and FuncMode properies

	ihppNode *treeTop;
	ihppNode *treeBottom;

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

	//IntraMode properties
	map<ADDRINT, ihppIntraModeContext*> intraModeContexts;

	//Methods

	ihppThreadContextClass(PIN_THREAD_UID tid, ADDRINT startFuncAddr, ADDRINT stopFuncAddr);

	~ihppThreadContextClass();

	inline bool canPopStack();
	inline bool popShadowStack();

	ihppIntraModeContext *getFunctionCtx(ADDRINT funcAddr);
	ihppIntraModeContext *getCurrentFunctionCtx();

	ADDRINT getCurrentFunction();
	ihppIntraModeContext *setCurrentFunction(ADDRINT currFunc);
	string getCurrentFunctionName(); 
};

inline bool ihppThreadContextClass::canPopStack() {
	
#if defined(_WIN32) && ENABLE_WIN32_MAIN_ALIGNMENT
	
	return shadowStack.size() > 0 && 
			(__tmainCRTStartup_stack_size == (unsigned)-1 || shadowStack.size()-1 >= __tmainCRTStartup_stack_size+1);

#else

	return shadowStack.size() > 0;

#endif
}

inline bool ihppThreadContextClass::popShadowStack() 
{
	if (canPopStack()) {
			
		funcMode_ret();
		return true;
	} 
		
	return false;
}

inline ihppThreadContextClass::ihppThreadContextClass(PIN_THREAD_UID tid, ADDRINT startFuncAddr, ADDRINT stopFuncAddr)
	 :  ihppAbstractContext(),  currentFunction(0), threadID(tid)
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

		ihppThreadContextClass::startFuncAddr = startFuncAddr;
		ihppThreadContextClass::stopFuncAddr = stopFuncAddr;

		if (startFuncAddr)
			haveToTrace = false;
		else
			haveToTrace = true;
	}


inline ihppIntraModeContext *ihppThreadContextClass::getCurrentFunctionCtx() 
{ 
	return getFunctionCtx(currentFunction); 
}


inline ADDRINT ihppThreadContextClass::getCurrentFunction() 
{ 
	return currentFunction; 
}

inline ihppIntraModeContext *ihppThreadContextClass::setCurrentFunction(ADDRINT currFunc)
{
	currentFunction=currFunc; 
	return getCurrentFunctionCtx();
}

inline ihppThreadContextClass::~ihppThreadContextClass()
{
	for (map<ADDRINT, ihppIntraModeContext*>::iterator it = intraModeContexts.begin(); it != intraModeContexts.end(); it++)
		delete it->second;
}

inline ihppIntraModeContext *ihppThreadContextClass::getFunctionCtx(ADDRINT funcAddr) 
{

	if (intraModeContexts.find(funcAddr) == intraModeContexts.end()) {
	
		ihppIntraModeContext *intraCtx;
		intraCtx = new ihppIntraModeContext(funcAddr);
		intraModeContexts[funcAddr] = intraCtx;
		return intraCtx;
	}

	return intraModeContexts[funcAddr];
}



#endif