
#ifdef _WIN32
#define KCCF_LIB_FILE "kCCF.dll"
#else
#define KCCF_LIB_FILE "kCCF.so"
#endif


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

KNOB<bool> showCalls(KNOB_MODE_WRITEONCE, "pintool", "showCalls", "0", "");
KNOB<bool> showBlocks(KNOB_MODE_WRITEONCE, "pintool", "showBlocks", "0", "");
KNOB<bool> showFuncs(KNOB_MODE_WRITEONCE, "pintool", "showFuncs", "0", "");
KNOB<bool> funcMode(KNOB_MODE_WRITEONCE, "pintool", "funcMode", "0", "");
KNOB<bool> blockMode(KNOB_MODE_WRITEONCE, "pintool", "blockMode", "0", "");
KNOB<bool> tradMode(KNOB_MODE_WRITEONCE, "pintool", "tradMode", "0", "");
KNOB<bool> joinThreads(KNOB_MODE_WRITEONCE, "pintool", "joinThreads", "0", "");
KNOB<bool> rollLoops(KNOB_MODE_WRITEONCE, "pintool", "rollLoops", "0", "");
KNOB<bool> disasm(KNOB_MODE_WRITEONCE, "pintool", "disasm", "0", "");

KNOB<string> startFunc(KNOB_MODE_WRITEONCE, "pintool", "startFunc", "-none-", "");
KNOB<string> stopFunc(KNOB_MODE_WRITEONCE, "pintool", "stopFunc", "-none-", "");

void setOptions() 
{
	globalSharedContext->startFuncName = startFunc.Value();
	globalSharedContext->stopFuncName = stopFunc.Value();
	globalSharedContext->purgeFuncs = purge.Value();

	globalSharedContext->joinThreads = joinThreads.Value();
	globalSharedContext->rollLoops = rollLoops.Value();

	globalSharedContext->showkSF = showkSF.Value();
	globalSharedContext->showkSF2 = showkSF2.Value();
	globalSharedContext->showkCCF = showkCCF.Value();
	globalSharedContext->showFuncs = showFuncs.Value();
	globalSharedContext->showBlocks = showBlocks.Value();
	globalSharedContext->showCalls = showCalls.Value();
	globalSharedContext->disasm = disasm.Value();
}



void showHelp()
{
	
	cout << endl << endl;
	cout << "k-Calling Context Forest profiling pintool\n" << endl;
	cout << "-------------------------------------------\n" << endl;
	cout << "Syntax: <PIN> -t " << KCCF_LIB_FILE << " <WORKING MODE> [ -f <func1> [-f <func2> [...]] ]\n"; 
	cout << "\t\t\t[ -k <K_VALUE> ] [-outfile <FILE>] <SHOW OPTIONS> <OTHER OPTIONS>" << endl << endl; 
	
	cout << "\nWorking modes: " << endl;
	cout << "\t -funcMode" << endl;
	cout << "\t -blockMode" << endl;
	cout << "\t -tradMode" << endl;
	
	cout << "\nShow options:\n";
	cout << "\t-ksf: shows the k-Slab Forest" << endl;
	cout << "\t-kccf: shows the k-Calling Context Forest" << endl;
	cout << "\t-showFuncs: shows function's list" << endl;
	cout << "\t-showBlocks: shows block's list" << endl;
	cout << "\t-disasm: shows disassembly in the section 'All basic blocks'" << endl;

	cout << "\nOther options:\n";
	cout << "\t-joinThreads: k Slab Forests of all thread will be joined" << endl;
	cout << "\t-rollLoops: when building the kSF in TradMode, loops will be rolled" << endl;

	cout << endl << endl;
}

bool checkOptions() 
{
	if ((  funcMode.Value() && (blockMode.Value() || tradMode.Value())  ) ||
		(  blockMode.Value() && (funcMode.Value() || tradMode.Value())  ) ||
		(  tradMode.Value() && (blockMode.Value() || funcMode.Value())  )) 
	{

		showHelp();
		return false;
	}

	if (!showkSF.Value() && !showkCCF.Value() && !showCalls.Value() && !showFuncs.Value() && !showBlocks.Value()) {
	
		showHelp();
		return false;
	} 


	if (disasm.Value() && !showBlocks.Value()) {
	
		cerr << "-disasm option only applicable with -showBlocks option." << endl;
		return false;
	}

#ifdef _WIN32
	
	if ( !singleFunctions.NumberOfValues() && !experimental.Value() ) {
	
		cerr << "Under Windows systems full tracing is an experimental feature:" << endl;
		cerr << "results MAY BE WRONG. To try it, use -experimental option." << endl;
		cerr << "Tip: with -purge option, sometimes results can be better.\n" << endl;
		return false;
	}
#endif

	if (purge.Value() && singleFunctions.NumberOfValues()) {
	
		cerr << "Purge option is available only in full tracing mode.\n";
		return false;
	}

	if (rollLoops.Value() && !tradMode.Value()) {
	
		cerr << "Roll loops option can be applied only with -tradMode option.\n";
		return false;
	}


	return true;
}

