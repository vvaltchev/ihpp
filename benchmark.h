
#ifndef __BENCHMARK_MACROS__
#define __BENCHMARK_MACROS__

#if KCCF_BENCHMARK 

	extern bool benchmark;
	extern bool usePinThread;
	extern bool useCurrFunc;

	extern PIN_THREAD_UID __benchmarkThread;
	extern ADDRINT __benchmarkFunc;


	#define BENCHMARK_VARS			unsigned int nodes_created,empty_nodes_created,nodes_copied,forests_copied;
	#define BENCHMARK_INIT_VARS		nodes_created=empty_nodes_created=nodes_copied=forests_copied=0;
	#define BENCHMARK_ON			benchmark=true;
	#define BENCHMARK_OFF			benchmark=false;

	#define BENCHMARK_SET_THREAD(tid)       usePinThread=false; __benchmarkThread=(tid);
	#define BENCHMARK_SET_FUNC(addr)		useCurrFunc=false; __benchmarkFunc=(addr);

	#define __BM_GET_THREAD		(usePinThread ? PIN_ThreadUid() : __benchmarkThread)
	#define __BM_CURR_TH		(globalSharedContext->getThreadCtx(__BM_GET_THREAD))
	#define __BM_GET_FUNC		(useCurrFunc ? __BM_CURR_TH->getCurrentFunction() : __benchmarkFunc)
	#define __BM_CURR_FUNC		(__BM_CURR_TH->getFunctionCtx(__BM_GET_FUNC))
	#define __BM_TRADMODE		(globalSharedContext->WorkingMode() == TradMode)


#else

	#define BENCHMARK_VARS
	#define BENCHMARK_INIT_VARS
	#define BENCHMARK_ON 
	#define BENCHMARK_OFF

	#define BENCHMARK_SET_THREAD(tid)		
	#define BENCHMARK_SET_FUNC(addr)				

#endif 

inline void BM_inc_nodes_created();
inline void BM_inc_empty_nodes_created();
inline void BM_inc_nodes_copied();
inline void BM_inc_forests_copied();

class BenchmarkObj {
	
public:

	BENCHMARK_VARS

#if KCCF_BENCHMARK
	void _BM_sumBenchmarkInfo(BenchmarkObj *obj) {
	
		nodes_created+=obj->nodes_created;
		empty_nodes_created+=obj->empty_nodes_created;
		nodes_copied+=obj->nodes_copied;
		forests_copied+=obj->forests_copied;
	}
#endif

};

#endif

#ifdef __KCCF_HEADER__

#if KCCF_BENCHMARK 

	inline void BM_inc_nodes_created() 
	{ if (benchmark) ((!__BM_TRADMODE || !__BM_GET_FUNC) ? __BM_CURR_TH->nodes_created++ : __BM_CURR_FUNC->nodes_created++);  }

	inline void BM_inc_empty_nodes_created()
	{ if (benchmark) ((!__BM_TRADMODE || !__BM_GET_FUNC) ? __BM_CURR_TH->empty_nodes_created++ : __BM_CURR_FUNC->empty_nodes_created++);  }
	
	inline void BM_inc_nodes_copied()
	{ if (benchmark) ((!__BM_TRADMODE || !__BM_GET_FUNC) ? __BM_CURR_TH->nodes_copied++ : __BM_CURR_FUNC->nodes_copied++);  }
	
	inline void BM_inc_forests_copied()
	{ if (benchmark) ((!__BM_TRADMODE || !__BM_GET_FUNC) ? __BM_CURR_TH->forests_copied++ : __BM_CURR_FUNC->forests_copied++);  }

#else

	inline void BM_inc_nodes_created() { }

	inline void BM_inc_empty_nodes_created() { }
	
	inline void BM_inc_nodes_copied() { }
	
	inline void BM_inc_forests_copied() { }

#endif


#endif