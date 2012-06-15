
#if defined(OPTIONS_MOUDLE) && !defined(_OPTIONS_VARS_DEFINED) 

#define _OPTIONS_VARS_DEFINED

KNOB<unsigned int> kparameter(KNOB_MODE_WRITEONCE, "pintool", "k", "3", "");
KNOB<string> singleFunctions(KNOB_MODE_APPEND, "pintool", "f", "", "");
KNOB<string> outFileName(KNOB_MODE_WRITEONCE, "pintool", "outfile", "out.txt", "");

KNOB<bool> showkSF(KNOB_MODE_WRITEONCE, "pintool", "ksf", "0", "");
KNOB<bool> showkSF2(KNOB_MODE_WRITEONCE, "pintool", "ksf2", "0", "");
KNOB<bool> showkCCF(KNOB_MODE_WRITEONCE, "pintool", "kccf", "0", "");

////////
KNOB<bool> purge(KNOB_MODE_WRITEONCE, "pintool", "purge", "0", "");
KNOB<bool> experimental(KNOB_MODE_WRITEONCE, "pintool", "experimental", "0", "");
/////////

KNOB<bool> funcMode(KNOB_MODE_WRITEONCE, "pintool", "funcMode", "0", "");
KNOB<bool> blockMode(KNOB_MODE_WRITEONCE, "pintool", "blockMode", "0", "");
KNOB<bool> tradMode(KNOB_MODE_WRITEONCE, "pintool", "tradMode", "0", "");
KNOB<bool> showCalls(KNOB_MODE_WRITEONCE, "pintool", "showCalls", "0", "");
KNOB<bool> showBlocks(KNOB_MODE_WRITEONCE, "pintool", "showBlocks", "0", "");
KNOB<bool> showFuncs(KNOB_MODE_WRITEONCE, "pintool", "showFuncs", "0", "");
KNOB<bool> joinThreads(KNOB_MODE_WRITEONCE, "pintool", "joinThreads", "0", "");
KNOB<bool> rollLoops(KNOB_MODE_WRITEONCE, "pintool", "rollLoops", "0", "");
KNOB<bool> disasm(KNOB_MODE_WRITEONCE, "pintool", "disasm", "0", "");
KNOB<bool> kinf(KNOB_MODE_WRITEONCE, "pintool", "kinf", "0", "");

KNOB<string> startFunc(KNOB_MODE_WRITEONCE, "pintool", "startFunc", "-none-", "");
KNOB<string> stopFunc(KNOB_MODE_WRITEONCE, "pintool", "stopFunc", "-none-", "");

#endif


#ifndef KCCF_OPTIONS_HEADER
#define KCCF_OPTIONS_HEADER

#include "pin.H"

#ifdef _WIN32
#define KCCF_LIB_FILE "kCCF.dll"
#else
#define KCCF_LIB_FILE "kCCF.so"
#endif


#ifndef _OPTIONS_VARS_DEFINED

extern KNOB<unsigned int> kparameter;
extern KNOB<string> singleFunctions;
extern KNOB<string> outFileName;

extern KNOB<bool> showkSF;
extern KNOB<bool> showkSF2;
extern KNOB<bool> showkCCF;

////////
extern KNOB<bool> purge;
extern KNOB<bool> experimental;
/////////

extern KNOB<bool> funcMode;
extern KNOB<bool> blockMode;
extern KNOB<bool> tradMode;
extern KNOB<bool> showCalls;
extern KNOB<bool> showBlocks;
extern KNOB<bool> showFuncs;
extern KNOB<bool> joinThreads;
extern KNOB<bool> rollLoops;
extern KNOB<bool> disasm;
extern KNOB<bool> kinf;

extern KNOB<string> startFunc;
extern KNOB<string> stopFunc;

#endif

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
};



void setOptions();
void showHelp();
bool checkOptions();

#endif