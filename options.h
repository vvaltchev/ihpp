
#ifndef __KCCF_OPTIONS_HEADER__
#define __KCCF_OPTIONS_HEADER__

#include "pin.H"

#ifdef _WIN32
#define KCCF_LIB_FILE "kCCF.dll"
#else
#define KCCF_LIB_FILE "kCCF.so"
#endif


enum WorkingModeType { WM_FuncMode, WM_IntraMode, WM_InterProcMode };

const char * const WorkingModesString[] = { "funcMode", "intraMode", "interMode" };

class optionsClass {

public:

	bool joinThreads;
	bool rollLoops;

	bool showkSF;
	bool showkSF2;
	bool showkCCF;
	bool showFuncs;
	bool showBlocks;
	bool showCalls;
	bool blocksDisasm;
	bool funcsDisasm;
	bool kinf;
	bool xmloutput;
	bool unrollRec;
	bool insTracing;

	string startFuncName;
	string stopFuncName;
	
	string tracingFuncList;

	optionsClass() 
	{
		joinThreads=false;
		rollLoops=false;
		showkSF=false;
		showkSF2=false;
		showkCCF=false;
		showFuncs=false;
		showBlocks=false;
		showCalls=false;
		blocksDisasm=false;
		funcsDisasm=false;
		kinf=false;
		xmloutput=false;
		unrollRec=false;
		insTracing=false;
	}

	void initFromGlobalOptions();

	static void showHelp();
	static bool checkOptions();
	static unsigned int getGlobalKVal();
	static WorkingModeType getGlobalWM();
	static const char *getOutfileName();
};





#endif