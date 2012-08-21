
#include "config.h"
#include "debug.h"
#include "ihpp.h"

#define OPTIONS_MOUDLE

#include "options.h"

using namespace std;

//Static symbols... 

static KNOB<unsigned int> kparameter(KNOB_MODE_WRITEONCE, "pintool", "k", "3", "");
static KNOB<string> singleFunctions(KNOB_MODE_APPEND, "pintool", "f", "", "");
static KNOB<string> outFileName(KNOB_MODE_WRITEONCE, "pintool", "outfile", "out.txt", "");

static KNOB<bool> showkSF(KNOB_MODE_WRITEONCE, "pintool", "ksf", "0", "");
static KNOB<bool> showkSF2(KNOB_MODE_WRITEONCE, "pintool", "ksf2", "0", "");
static KNOB<bool> showkCCF(KNOB_MODE_WRITEONCE, "pintool", "kccf", "0", "");

////////
//static KNOB<bool> purge(KNOB_MODE_WRITEONCE, "pintool", "purge", "0", "");
static KNOB<bool> experimental(KNOB_MODE_WRITEONCE, "pintool", "experimental", "0", "");
/////////


//working modes options
static KNOB<bool> opt_funcMode(KNOB_MODE_WRITEONCE, "pintool", "funcMode", "0", "");        //0
static KNOB<bool> opt_intraMode(KNOB_MODE_WRITEONCE, "pintool", "intraMode", "0", "");      //1
static KNOB<bool> opt_interProcMode(KNOB_MODE_WRITEONCE, "pintool", "interMode", "0", "");  //2

static KNOB<bool> showCalls(KNOB_MODE_WRITEONCE, "pintool", "showCalls", "0", "");
static KNOB<bool> showBlocks(KNOB_MODE_WRITEONCE, "pintool", "showBlocks", "0", "");
static KNOB<bool> showFuncs(KNOB_MODE_WRITEONCE, "pintool", "showFuncs", "0", "");
static KNOB<bool> joinThreads(KNOB_MODE_WRITEONCE, "pintool", "joinThreads", "0", "");
static KNOB<bool> rollLoops(KNOB_MODE_WRITEONCE, "pintool", "rollLoops", "0", "");

static KNOB<bool> blocksDisasm(KNOB_MODE_WRITEONCE, "pintool", "blocksDisasm", "0", "");
static KNOB<bool> funcsDisasm(KNOB_MODE_WRITEONCE, "pintool", "funcsDisasm", "0", "");

static KNOB<bool> kinf(KNOB_MODE_WRITEONCE, "pintool", "kinf", "0", "");
static KNOB<bool> xmloutput(KNOB_MODE_WRITEONCE, "pintool", "xml", "0", "");

static KNOB<string> startFunc(KNOB_MODE_WRITEONCE, "pintool", "startFunc", "--", "");
static KNOB<string> stopFunc(KNOB_MODE_WRITEONCE, "pintool", "stopFunc", "--", "");

//----------------------------------------------------------------------------------------



KNOB<string> &optionsClass::tracingFunctions() { 

	return singleFunctions; 
}

const char *optionsClass::getOutfileName() { 
	
	if (::outFileName.Value() == "out.txt" && ::xmloutput.Value())
		return "out.xml";

	return outFileName.Value().c_str(); 
}


unsigned int optionsClass::getGlobalKVal() {
	
	return ::kparameter.Value();
}

WorkingModeType optionsClass::getGlobalWM() {

	if (::opt_intraMode.Value())
		return WM_IntraMode;

	if (::opt_interProcMode.Value())
		return WM_InterProcMode;

	return WM_FuncMode;
}

void optionsClass::initFromGlobalOptions() 
{

	startFuncName = ::startFunc.Value();
	stopFuncName = ::stopFunc.Value();
	//purgeFuncs = ::purge.Value();

	joinThreads = ::joinThreads.Value();
	rollLoops = ::rollLoops.Value();

	showkSF = ::showkSF.Value();
	showkSF2 = ::showkSF2.Value();
	showkCCF = ::showkCCF.Value();
	showFuncs = ::showFuncs.Value();
	showBlocks = ::showBlocks.Value();
	showCalls = ::showCalls.Value();
	blocksDisasm = ::blocksDisasm.Value();
	funcsDisasm = ::funcsDisasm.Value();
	kinf = ::kinf.Value();
	xmloutput = ::xmloutput.Value();

	//automatic option implications
	if (rollLoops && ::opt_intraMode.Value()) {
		kinf = true;
	}
}



void optionsClass::showHelp()
{
	
	cout << endl << endl;
	cout << "IHPP: An Intraprocedural Hot Path Profiler\n" << endl;
	cout << "-------------------------------------------\n" << endl;
	cout << "Syntax: <PIN> -t " << KCCF_LIB_FILE << " <WORKING MODE> [ -f <func1> [-f <func2> [...]] ]\n"; 
	cout << "\t\t\t[ -k <K_VALUE> | -kinf ] [-outfile <FILE>] [ -xml ] <SHOW OPTIONS> <OTHER OPTIONS>" << endl << endl; 
	
	cout << "\nWorking modes: " << endl;
	cout << "\t -funcMode : procedure-level profiling [ default mode ]" << endl;
	cout << "\t -intraMode : intra-procedural basic block profiling" << endl;
	cout << "\t -interMode : inter-procedural basic block profiling" << endl;
	
	cout << "\nShow options:\n";
	cout << "\t-ksf: shows the k-Slab Forest" << endl;
	cout << "\t-kccf: shows the k-Calling Context Forest" << endl;
	cout << "\t-showFuncs: shows function's list" << endl;
	cout << "\t-showBlocks: shows block's list" << endl;
	cout << "\t-blocksDisasm: shows disassembly in the section 'All basic blocks'" << endl;
	cout << "\t-funcsDisasm: shows disassembly in the section 'All functions'" << endl;

	cout << "\nOther options:\n";
	cout << "\t-xml option produces the output file in xml format" << endl;
	cout << "\t-joinThreads: k Slab Forests of all thread will be joined" << endl;
	cout << "\t-rollLoops: when building the kSF in IntraMode, loops will be rolled (-kinf is implied)" << endl;

	cout << endl << endl;
}

bool optionsClass::checkOptions() 
{

	if ((  opt_funcMode.Value() && (opt_interProcMode.Value() || opt_intraMode.Value())  ) ||
		(  opt_interProcMode.Value() && (opt_funcMode.Value() || opt_intraMode.Value())  ) ||
		(  opt_intraMode.Value() && (opt_interProcMode.Value() || opt_funcMode.Value())  )) 
	{

		optionsClass::showHelp();
		return false;
	}

	if (!::showkSF.Value() && !::showkSF2.Value() && !::showkCCF.Value() && 
		!::showCalls.Value() && !::showFuncs.Value() && !::showBlocks.Value()) {
	
		optionsClass::showHelp();
		return false;
	} 

	if (::showBlocks.Value() && opt_funcMode.Value()) {
	
		cerr << "-showBlocks can't be used in funcMode." << endl;
		return false;
	}

	if (::blocksDisasm.Value() && !::showBlocks.Value()) {
	
		cerr << "-blocksDisasm option only applicable with -showBlocks option." << endl;
		return false;
	}

	

	if (::funcsDisasm.Value() && !::showFuncs.Value()) {
		
		cerr << "-funcsDisasm option only applicable with -showFuncs option." << endl;
		return false;	
	}

	if (::kinf.Value() && !::rollLoops.Value() && ::opt_intraMode.Value()) {
	
		cerr << "It's an error to use k = infinite (-kinf) without -rollLoops option in intraMode.\n";
		return false;
	}

	if (::kinf.Value() && ::opt_interProcMode.Value()) {
	
		cerr << "-kinf option can't be used with -interMode.\n";
		return false;
	}

#ifdef _WIN32
	
	if ( !singleFunctions.NumberOfValues() && !experimental.Value() ) {
	
		cerr << "Under Windows systems full tracing is an experimental feature:" << endl;
		cerr << "results MAY BE WRONG. To try it, use -experimental option." << endl;
		cerr << "Use -f <func> [ -f <func> ... ] to do a SELECTIVE tracing (much more reliable).\n" << endl;
		cerr << "Tip: full tracing combined with -startFunc <func> (es. main)" << endl;
		cerr << "and -stopFunc <func> (es. exit) may produce better results.\n" << endl;

		return false;
	}
#endif

/*
	if (::purge.Value() && ::singleFunctions.NumberOfValues()) {
	
		cerr << "Purge option is available only in full tracing mode.\n";
		return false;
	}
*/

	if (::rollLoops.Value() && !::opt_intraMode.Value()) {
	
		cerr << "Roll loops option can be applied only with -intraMode option.\n";
		return false;
	}


	return true;
}

