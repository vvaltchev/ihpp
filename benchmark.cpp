#include "config.h"
#include "pin.H"

#if IHPP_BENCHMARK

class ThreadContext;

bool benchmark=true;
bool usePinThread=true;
bool useCurrFunc=true;

ThreadContext *__benchmarkThread=0;
ADDRINT __benchmarkFunc=0;

#endif

#if USING_WINDOWS

namespace win {
    #include <windows.h>
}

double getMilliseconds() {

    return (double)win::GetTickCount();
}

#else

#include <time.h>
#include <sys/time.h>

double getMilliseconds() {

    timeval tv;
    gettimeofday(&tv, NULL);

    return tv.tv_sec*1000.0 + tv.tv_usec/1000.0;
}

#endif

