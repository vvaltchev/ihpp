
#ifndef __DEBUG_HEADER__
#define __DEBUG_HEADER__

#if CHECK_ASSERTS

	#ifdef NDEBUG
		#undef NDEBUG
	#endif

#else
 
	#ifndef NDEBUG
		#define NDEBUG
	#endif

#endif


#define __dcheck (globalSharedContext->showDebug)

#if DEBUG && DEBUG_SINGLE_INST


	#define dbg_single_inst_have_to_jmp()		if (__dcheck) cerr << "INS: FORCED LONGJMP! [cod0]\n";
	#define dbg_single_inst_false_jmp()			if (__dcheck) cerr << "INS: FALSE longjmp found, after: " << ctx->subcallcount << endl;

	#define dbg_single_inst_reg_jmp()			if (__dcheck) cerr << "\nINS: longjmp [cod1]: addr != last jump addr!!\n";\
												if (__dcheck) cerr << "------------------ curr RTN:          " << RTN_FindNameByAddress(currFuncAddr) << endl;\
												if (__dcheck) cerr << "------------------ last target addr:  " << RTN_FindNameByAddress(ctx->jumpTargetFuncAddr) << endl;

#else

	#define dbg_single_inst_have_to_jmp()		
	#define dbg_single_inst_false_jmp()			
	#define dbg_single_inst_reg_jmp()			
												
#endif

#if DEBUG && DEBUG_BRCALL

	#define dbg_brcall_jmp()					if (__dcheck) cerr << "\nINS [cod2]: PROBABLE longjmp from: " << currentFuncName << " a " << targetFuncName << endl;\
												if (__dcheck) cerr << "INS: insCat: " <<  CATEGORY_StringShort((CATEGORY)insCat) << endl;


	#define dbg_brcall_cantpop()				if (__dcheck) cerr << "INS: CAN'T pop()!!!\n";

	#define dbg_brcall_ifret()					if (__dcheck) if (insCat == XED_CATEGORY_RET) cerr << "INS: branchOrCall, RET INSTRUCTION" << endl;

	#define dbg_brcall_keepoldjumpaddr()		if (__dcheck) cerr << "INS: I'm in .text, but DON'T SET jump target on: " << targetFuncName;\
												if (__dcheck) cerr << ", but on: " << RTN_FindNameByAddress(ctx->lastJumpTargetFuncAddr) << endl;

	#define dbg_brcall_subcall()				if (__dcheck) cerr << "INS: subfunction internal CALL, by: " << currentFuncName << endl; \
												if (__dcheck) cerr << "INS: brachOrCall, CALL:     " << hex << (void*)targetFuncAddr << dec << endl; \
												if (__dcheck) cerr << "INS: exact target addr:     " << hex << (void*)targetAddr << dec << endl; \
												if (__dcheck) cerr << "INS: current function addr: " << hex << (void*)currentFuncAddr << dec << endl; \
												if (__dcheck) cerr << "INS: traceTarget: " << traceTarget << endl;

	#define dbg_brcall_tracejmp()				if (__dcheck) if (ctx->jumpTargetFuncAddr != targetFuncAddr) \
													cerr << "INS: current func: " << currentFuncName << ", setting jump to: " << targetFuncName << endl;

	#define dbg_brcall_fjmp_found1()			if (__dcheck) cerr << "INS: [cod9] found FORWARD jmp" << endl;
	#define dbg_brcall_fjmp_found2()			if (__dcheck) cerr << "INS: [cod9] can't pop, but record FORWARD jmp" << endl;
#else

	#define dbg_brcall_jmp()					
	#define dbg_brcall_cantpop()				
	#define dbg_brcall_ifret()					
	#define dbg_brcall_keepoldjumpaddr()		
	#define dbg_brcall_subcall()				
	#define dbg_brcall_tracejmp()	
	#define dbg_brcall_fjmp_found1()
	#define dbg_brcall_fjmp_found2()

#endif

#if DEBUG && DEBUG_IMGLOAD

	#define dbg_imgload_imgname()					if (__dcheck) cerr << "Image: " << IMG_Name(img) << endl;
	#define dbg_imgload_sectorname()				if (__dcheck) cerr << "Sector: " << SEC_Name(sec) << endl;
	#define dbg_imgload_funcname()					if (__dcheck) cerr << "Function: " << funcName << "(0x" << hex \
														 << (void*)funcAddr << dec << (trace?"): TRACING\n": "): SKIPPED\n");

#else
	#define dbg_imgload_imgname()		
	#define dbg_imgload_sectorname()	
	#define dbg_imgload_funcname()		
#endif

#if DEBUG && DEBUG_FUNC_TRACE

	#define dbg_functr_funcname()					if (__dcheck) cerr << "\nFUNC_MODE trace: " << fc->functionName() << ", stacksize: " << ctx->shadowStack.size() << endl;
	#define dbg_functr_stackptr()					if (__dcheck) cerr << "FUNC_MODE: reg_sp = " << reg_sp << endl << "FUNC_MODE: top sp = " << FUNCMODE_TOP_STACKPTR() << endl;

	#define dbg_functr_regsp_gt()					if (__dcheck) cerr << "FUNC_MODE: reg_sp >= topstackptr [cod4]\n";
	#define dbg_functr_pop()						if (__dcheck) cerr << "FUNC_MODE: pop()\n";
	#define dbg_functr_fjmps_set()					if (__dcheck) cerr << "FUNC_MODE: current func has " << ctx->lastfjmps+1 << " fjmps!\n";

#else

	#define dbg_functr_funcname()
	#define dbg_functr_stackptr()
	#define dbg_functr_regsp_gt()					
	#define dbg_functr_pop()						
	#define dbg_functr_fjmps_set()

#endif

#if DEBUG && DEBUG_FUNC_RET

	#define dbg_funcret_name()						if (__dcheck) cerr << "\nFUNC_MODE: func ret: " << globalCtx->allFuncs[ctx->getCurrentFunction()]->functionName() << endl;
	#define dbg_funcret_pop_err()					if (__dcheck) cerr << "FUNC_MODE: Can't pop(): stack size() == 1\n";
	#define dbg_funcret_pop()						if (__dcheck) cerr << "FUNC_MODE: RETURN -> pop()\n";
	#define dbg_funcret_stack_after_pop()			if (__dcheck) cerr << "FUNC_MODE: stack size after pop() = " << ctx->shadowStack.size() << endl;
	#define dbg_funcret_new_call()					if (__dcheck) if (!ctx->shadowStack.size()) cerr << "LAST RETURN, next call will be a NEW CALL\n";
	#define dbg_funcret_fjmps()						if (__dcheck) if (ctx->shadowStack.top().fjmps) cerr << "FUNC RET: [cod10], fjmps: " << ctx->shadowStack.top().fjmps << endl;

#else

	#define dbg_funcret_name()
	#define dbg_funcret_pop_err()
	#define dbg_funcret_pop()
	#define dbg_funcret_stack_after_pop()
	#define dbg_funcret_new_call()
	#define dbg_funcret_fjmps()

#endif

#if DEBUG && DEBUG_TRAD_TRACE

	#define dbg_tradtr_begin()						if (__dcheck) cerr << "\nTRAD_MODE: tracing block: " << *bb << ", addr: 0x" << hex << (void*)bb->blockAddress() << dec << endl; \
													if (__dcheck) cerr << "TRAD_MODE: reg_sp = " << reg_sp << endl;
	
	#define dbg_tradtr_nlog_skip()					if (__dcheck) cerr << "\nTRAD_MODE: SKIPPING block: " << *bb << endl;

	#define dbg_tradtr_longjmp()					if (__dcheck) cerr << "TRAD_MODE: probable longjmp [cod3]\n"; \
													if (__dcheck) cerr << "currFunc: " << RTN_FindNameByAddress( ctx->getCurrentFunction() ) << endl; \
													if (__dcheck) cerr << "blockFunc: " << RTN_FindNameByAddress( bb->functionAddr() ) << endl; \
													if (__dcheck) cerr << "Calling tradmode return procedure..\n";

	#define dbg_tradtr_begin_sp()					if (__dcheck) cerr << "begin stack size: " << tradCtx->shadowStack.size() << endl;
	#define dbg_tradtr_normal_trace()				if (__dcheck) cerr << "NORMAL traceObject\n";
	#define dbg_tradtr_end_sp()						if (__dcheck) cerr << "end stack size: " << tradCtx->shadowStack.size() << endl;
	#define dbg_tradtr_first_block()				if (__dcheck) cerr << "FIRST BLOCK\n";
	#define dbg_tradtr_first_call()					if (__dcheck) cerr << "FIRST FUNCTION CALL\n";

#else

	#define dbg_tradtr_begin()
	#define dbg_tradtr_nlog_skip()
	#define dbg_tradtr_longjmp()	
	#define dbg_tradtr_begin_sp()	
	#define dbg_tradtr_normal_trace()	
	#define dbg_tradtr_end_sp()			
	#define dbg_tradtr_first_block()	
	#define dbg_tradtr_first_call()		

#endif

#if DEBUG && DEBUG_TRAD_RET 

	#define dbg_tradret_begin()						if (__dcheck) cerr << "TRAD_MODE ret: " << ctx->getCurrentFunctionName() << endl;
	#define dbg_tradret_cantpop()					if (__dcheck) cerr << "TRAD_MODE ret: can't pop(), stack size() == 0\n";
	#define dbg_tradret_stackpop()					if (__dcheck) cerr << "TRAD_MODE shadowStack.pop()\n";
	#define dbg_tradret_lastret()					if (__dcheck) cerr << "TRAD_MODE: LAST RETURN, next call will be a NEW CALL\n";

#else

	#define dbg_tradret_begin()
	#define dbg_tradret_cantpop()
	#define dbg_tradret_stackpop()
	#define dbg_tradret_lastret()

#endif

#endif