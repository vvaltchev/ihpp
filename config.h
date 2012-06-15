
#ifndef __KCCF_CONFIG__
#define __KCCF_CONFIG__

#if !defined(_WIN32) && !defined(__unix__)
#error This tool builds only on Win32 and Unix-like systems
#endif

#define KCCF_BENCHMARK		0

#define DEBUG				0
#define CHECK_ASSERTS		0


#define DEBUG_SINGLE_INST	1
#define DEBUG_BRCALL		1
#define DEBUG_IMGLOAD		1
#define DEBUG_FUNC_TRACE	1
#define DEBUG_FUNC_RET		1
#define DEBUG_TRAD_TRACE	1
#define DEBUG_TRAD_RET		1

#if defined(_WIN32) 
	#define ENABLE_INS_TRACING									1
	#define ENABLE_SUBCALL_CHECK								1
	#define ENABLE_RELY_ON_SP_CHECK								0
	#define ENABLE_WIN32_FULLTRACE_TARGET_TRACING_LONGJMP		0
	#define ENABLE_WIN32_MAIN_ALIGNMENT							1
	#define ENABLE_INS_FORWARD_JMP_RECOGNITION					1
#else
	#define ENABLE_INS_TRACING									0
	#define ENABLE_SUBCALL_CHECK								0
	#define ENABLE_RELY_ON_SP_CHECK								1
	#define ENABLE_WIN32_FULLTRACE_TARGET_TRACING_LONGJMP		0
	#define ENABLE_WIN32_MAIN_ALIGNMENT							0
	#define ENABLE_INS_FORWARD_JMP_RECOGNITION					0
#endif

#endif