/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if ENABLE_INS_TRACING

void simpleIns(kCCFContextClass *globalCtx, FunctionObj *fc, ADDRINT insAddr, ADDRINT insCat, ADDRINT sp) {

	kCCFThreadContextClass *ctx;
	
	ctx = globalCtx->getThreadCtx(PIN_ThreadUid());	

	ADDRINT currFuncAddr = fc->functionAddress();
	string currFuncName = fc->functionName();

	//cerr << currFuncName << "(): " << CATEGORY_StringShort(insCat) << endl;
	
	if (insAddr == fc->functionAddress()) {
	
		FunctionObjTrace(fc, sp);

		return;
	}
	
	/*
	if (ctx->jumpTargetFuncAddr && currFuncAddr != ctx->jumpTargetFuncAddr) { 
	
		dbg_single_inst_reg_jmp();

		if (ctx->shadowStack.size() > 1)
				funcMode_ret(globalCtx);
	}
	*/

}

void branch(kCCFContextClass *globalCtx, FunctionObj *fc, ADDRINT insAddr, ADDRINT targetAddr, ADDRINT insCat, ADDRINT sp) {
	
	RTN rtn;
	ADDRINT currentFuncAddr=0;
	ADDRINT targetFuncAddr=0;

	string currentFuncName;
	string targetFuncName;
	
	kCCFThreadContextClass *ctx;
	
	ctx = globalCtx->getThreadCtx(PIN_ThreadUid());	

	//getCurrentAndTargetFunc(insAddr, targetAddr, currentFuncAddr, targetFuncAddr, currentFuncName, targetFuncName);

	currentFuncAddr = fc->functionAddress();
	currentFuncName = fc->functionName();

	PIN_LockClient();

	rtn = RTN_FindByAddress(targetAddr);

	if (RTN_Valid(rtn)) {

		targetFuncName = RTN_Name(rtn);
		targetFuncAddr = RTN_Address(rtn);
	}

	PIN_UnlockClient();

	if (!targetFuncAddr) {
	

		cerr << "[cod3]: no target info\n";
		return;
	}

	//cerr << "jump target: " << targetFuncName << endl;


	simpleIns(globalCtx, fc, insAddr, insCat, sp);

	if (insCat == XED_CATEGORY_RET) {
	
	
		if (ctx->shadowStack.size() > 1)
			funcMode_ret();


	}
	
	if (insCat == XED_CATEGORY_UNCOND_BR)
		if (targetFuncAddr != currentFuncAddr && !FUNC_IS_TEXT(currentFuncAddr)) {
	
			dbg_brcall_jmp();

			if (ctx->shadowStack.size() > 1) {
				
				
#if ENABLE_INS_FORWARD_JMP_RECOGNITION

				for (size_t i=1; i < ctx->shadowStack.size(); i++) {

					ADDRINT beforeLastTop = ctx->shadowStack.top(i).treeTop->getKey();
					
					if (targetFuncAddr == beforeLastTop) {
				
						funcMode_ret();
						//ctx->popShadowStack();
						break;
					} 

				}
#else
				funcMode_ret(globalCtx);
				//ctx->popShadowStack();
#endif
			}
			
		} 

	

	if (FUNC_IS_TEXT(currentFuncAddr)) {
	
		dbg_brcall_keepoldjumpaddr();

		ctx->jumpTargetFuncAddr = ctx->lastJumpTargetFuncAddr;
		return;
	}

	ctx->lastJumpTargetFuncAddr = ctx->jumpTargetFuncAddr;
	ctx->jumpTargetFuncAddr = targetFuncAddr;



}

void InstructionIns2(INS ins, void *arg)
{

	RTN rtn;
	ADDRINT insAddr;
	FunctionObj *fc;
	map<ADDRINT, FunctionObj*>::iterator it;

	kCCFContextClass *globalCtx = static_cast<kCCFContextClass *>(arg);

	insAddr = INS_Address(ins);

	rtn = RTN_FindByAddress(insAddr);

	if (!RTN_Valid(rtn))
		return;


	ADDRINT funcAddr = RTN_Address(rtn);
	string funcName = RTN_Name(rtn);

	it = globalCtx->allFuncs.find(funcAddr);

	if (it == globalCtx->allFuncs.end()) {
	
		fc = new FunctionObj(funcAddr, funcName);
		globalCtx->allFuncs[funcAddr]=fc;

	} else {
	
		fc = it->second;
	}


	
	if (INS_IsBranchOrCall(ins) || INS_IsRet(ins)) {
	
		INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)branch, 
		IARG_PTR, globalCtx, IARG_PTR, fc, IARG_PTR, insAddr, IARG_BRANCH_TARGET_ADDR,
		IARG_PTR, INS_Category(ins),  IARG_REG_VALUE, REG_STACK_PTR, IARG_END);
	
	} else {

		INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)simpleIns, 
		IARG_PTR, globalCtx, IARG_PTR, fc, IARG_PTR, insAddr, 
		IARG_PTR, INS_Category(ins),  IARG_REG_VALUE, REG_STACK_PTR, IARG_END);
	
	}
	

		


	//cerr << "func: " << currFuncName << ", " << CATEGORY_StringShort(INS_Category(ins)) << endl;
}

VOID ImageLoad2(IMG img, VOID *v) {

	
	kCCFContextClass *globalCtx = static_cast<kCCFContextClass *>(v);
	bool mainImage = IMG_IsMainExecutable(img);
	
	dbg_imgload_imgname();

	for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec)) {

		dbg_imgload_sectorname();

		for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn)) {
			
			cerr << "func load: " << RTN_Name(rtn) << endl; 

			RTN_Open(rtn);

			for( INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins) )
				InstructionIns2(ins, globalCtx);			

			RTN_Close(rtn);


		}
	}

	
	cerr << "imageload2 OK\n";
}


#endif