

VOID FunctionObjTrace(FunctionObj *fc, kCCFContextClass *globalCtx, ADDRINT reg_sp);

void PIN_FAST_ANALYSIS_CALL funcMode_ret(kCCFContextClass *globalCtx);

inline void funcMode_Ret_wrapper() { funcMode_ret(globalSharedContext); }


VOID tradModeBlockTrace(TracingObject<ADDRINT> *to, kCCFContextClass *globalCtx, ADDRINT reg_sp);

void PIN_FAST_ANALYSIS_CALL tradMode_ret(kCCFContextClass *globalCtx);


#if ENABLE_INS_TRACING

void PIN_FAST_ANALYSIS_CALL singleInstruction(kCCFContextClass *globalCtx, ADDRINT currFuncAddr, ADDRINT insCat);

void branchOrCall(kCCFContextClass *globalCtx, ADDRINT insAddr, ADDRINT targetAddr, ADDRINT insCat);

#endif