


#if CHECK_ASSERTS

	#ifdef NDEBUG
		#undef NDEBUG
	#endif

#else
 
	#ifndef NDEBUG
		#define NDEBUG
	#endif

#endif


#if DEBUG && DEBUG_SINGLE_INST


	#define dbg_single_inst_have_to_jmp()		cerr << "INS: FORCED LONGJMP! [cod0]\n";
	#define dbg_single_inst_false_jmp()			cerr << "INS: FALSE longjmp found, after: " << ctx->subcallcount << endl;

	#define dbg_single_inst_reg_jmp()			cerr << "\nINS: longjmp [cod1]: addr != last jump addr!!\n";\
												cerr << "------------------ curr RTN:          " << RTN_FindNameByAddress(currFuncAddr) << endl;\
												cerr << "------------------ last target addr:  " << RTN_FindNameByAddress(ctx->jumpTargetFuncAddr) << endl;

#else

	#define dbg_single_inst_have_to_jmp()		
	#define dbg_single_inst_false_jmp()			
	#define dbg_single_inst_reg_jmp()			
												
#endif

#if DEBUG && DEBUG_BRCALL

	#define dbg_brcall_jmp()					cerr << "\nINS [cod2]: PROBABLE longjmp from: " << currentFuncName << " a " << targetFuncName << endl;\
												cerr << "INS: insCat: " <<  CATEGORY_StringShort((CATEGORY)insCat) << endl;


	#define dbg_brcall_cantpop()				cerr << "INS: CAN'T pop()!!!\n";

	#define dbg_brcall_ifret()					if (insCat == XED_CATEGORY_RET) cerr << "INS: branchOrCall, RET INSTRUCTION" << endl;

	#define dbg_brcall_keepoldjumpaddr()		cerr << "INS: I'm in .text, but DON'T SET jump target on: " << targetFuncName;\
												cerr << ", but on: " << RTN_FindNameByAddress(ctx->lastJumpTargetFuncAddr) << endl;

	#define dbg_brcall_subcall()				cerr << "INS: subfunction internal CALL, by: " << currentFuncName << endl; \
												cerr << "INS: brachOrCall, CALL:     " << hex << (void*)targetFuncAddr << dec << endl; \
												cerr << "INS: exact target addr:     " << hex << (void*)targetAddr << dec << endl; \
												cerr << "INS: current function addr: " << hex << (void*)currentFuncAddr << dec << endl; \
												cerr << "INS: traceTarget: " << traceTarget << endl;

	#define dbg_brcall_tracejmp()				if (ctx->jumpTargetFuncAddr != targetFuncAddr) \
													cerr << "INS: current func: " << currentFuncName << ", setting jump to: " << targetFuncName << endl;

#else

	#define dbg_brcall_jmp()					
	#define dbg_brcall_cantpop()				
	#define dbg_brcall_ifret()					
	#define dbg_brcall_keepoldjumpaddr()		
	#define dbg_brcall_subcall()				
	#define dbg_brcall_tracejmp()				

#endif

#if DEBUG && DEBUG_IMGLOAD

	#define dbg_imgload_imgname()					cerr << "Image: " << IMG_Name(img) << endl;
	#define dbg_imgload_sectorname()				cerr << "Sector: " << SEC_Name(sec) << endl;
	#define dbg_imgload_funcname()					cerr << "Function: " << funcName << "(0x" << hex \
														 << (void*)funcAddr << dec << (trace?"): TRACING\n": "): SKIPPED\n");

#else
	#define dbg_imgload_imgname()		
	#define dbg_imgload_sectorname()	
	#define dbg_imgload_funcname()		
#endif

#if DEBUG && DEBUG_FUNC_TRACE

	#define dbg_functr_funcname()					cerr << "\nFUNC_MODE trace: " << fc->functionName() << ", stacksize: " << ctx->shadowStack.size() << endl;
	#define dbg_functr_stackptr()					cerr << "FUNC_MODE: reg_sp = " << reg_sp << endl << "FUNC_MODE: top sp = " << FUNCMODE_TOP_STACKPTR() << endl;

	#define dbg_functr_regsp_gt()					cerr << "FUNC_MODE: reg_sp >= topstackptr [cod4]\n";
	#define dbg_functr_pop()						cerr << "FUNC_MODE: pop()\n";

#else

	#define dbg_functr_funcname()
	#define dbg_functr_stackptr()
	#define dbg_functr_regsp_gt()					
	#define dbg_functr_pop()						

#endif

#if DEBUG && DEBUG_FUNC_RET

	#define dbg_funcret_name()						cerr << "\nFUNC_MODE: func ret: " << globalCtx->allFuncs[ctx->getCurrentFunction()]->functionName() << endl;
	#define dbg_funcret_pop_err()					cerr << "FUNC_MODE: Can't pop(): stack size() == 1\n";
	#define dbg_funcret_pop()						cerr << "FUNC_MODE: RETURN -> pop()\n";
	#define dbg_funcret_stack_after_pop()			cerr << "FUNC_MODE: stack size after pop() = " << ctx->shadowStack.size() << endl;
	#define dbg_funcret_new_call()					if (!ctx->shadowStack.size()) cerr << "LAST RETURN, next call will be a NEW CALL\n";
	#define dbg_funcret_fjmps()						if (ctx->shadowStack.top().fjmps) cerr << "FUNC RET: [cod10], fjmps: " << ctx->shadowStack.top().fjmps << endl;

#else

	#define dbg_funcret_name()
	#define dbg_funcret_pop_err()
	#define dbg_funcret_pop()
	#define dbg_funcret_stack_after_pop()
	#define dbg_funcret_new_call()
	#define dbg_funcret_fjmps()

#endif

#if DEBUG && DEBUG_TRAD_TRACE

	#define dbg_tradtr_begin()						cerr << "\nTRAD_MODE: tracing block: " << *bb << ", addr: 0x" << hex << (void*)bb->blockAddress() << dec << endl; \
													cerr << "TRAD_MODE: reg_sp = " << reg_sp << endl;
	
	#define dbg_tradtr_nlog_skip()					cerr << "\nTRAD_MODE: SKIPPING block: " << *bb << endl;

	#define dbg_tradtr_longjmp()					cerr << "TRAD_MODE: probable longjmp [cod3]\n"; \
													cerr << "currFunc: " << RTN_FindNameByAddress( ctx->getCurrentFunction() ) << endl; \
													cerr << "blockFunc: " << RTN_FindNameByAddress( bb->functionAddr() ) << endl; \
													cerr << "Calling tradmode return procedure..\n";

	#define dbg_tradtr_begin_sp()					cerr << "begin stack size: " << tradCtx->shadowStack.size() << endl;
	#define dbg_tradtr_normal_trace()				cerr << "NORMAL traceObject\n";
	#define dbg_tradtr_end_sp()						cerr << "end stack size: " << tradCtx->shadowStack.size() << endl;
	#define dbg_tradtr_first_block()				cerr << "FIRST BLOCK\n";
	#define dbg_tradtr_first_call()					cerr << "FIRST FUNCTION CALL\n";

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

	#define dbg_tradret_begin()						cerr << "TRAD_MODE ret: " << currFuncName << endl;
	#define dbg_tradret_cantpop()					cerr << "TRAD_MODE ret: can't pop(), stack size() == 0\n";
	#define dbg_tradret_stackpop()					cerr << "TRAD_MODE shadowStack.pop()\n";
	#define dbg_tradret_lastret()					cerr << "TRAD_MODE: LAST RETURN, next call will be a NEW CALL\n";

#else

	#define dbg_tradret_begin()
	#define dbg_tradret_cantpop()
	#define dbg_tradret_stackpop()
	#define dbg_tradret_lastret()

#endif
