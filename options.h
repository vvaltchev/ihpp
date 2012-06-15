
#ifndef __KCCF_OPTIONS_HEADER__
#define __KCCF_OPTIONS_HEADER__

#include "pin.H"

#ifdef _WIN32
#define KCCF_LIB_FILE "kCCF.dll"
#else
#define KCCF_LIB_FILE "kCCF.so"
#endif


enum WorkingModeType { FuncMode, BlockMode, TradMode };

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
	bool purgeFuncs;
	bool disasm;
	bool kinf;

	string startFuncName;
	string stopFuncName;
	
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
		purgeFuncs=false;	
		disasm=false;
		kinf=false;
	}

	void initFromGlobalOptions();

	static void showHelp();
	static bool checkOptions();
	static unsigned int getGlobalKVal();
	static WorkingModeType getGlobalWM();
	static KNOB<string> &tracingFunctions();
	static const char *getOutfileName();
};





#endif