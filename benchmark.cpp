#include "config.h"
#include "pin.H"

#if IHPP_BENCHMARK 

	bool benchmark=true;
	bool usePinThread=true;
	bool useCurrFunc=true;

	PIN_THREAD_UID __benchmarkThread=0;
	ADDRINT __benchmarkFunc=0;

#endif