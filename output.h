
#ifndef __OUTPUT_HEADER__
#define __OUTPUT_HEADER__

#include <iomanip>
#include "util.h"

VOID Fini(INT32 code, VOID *v);
void funcTraceDebugDump(kCCFContextClass *globalCtx, FunctionObj *fc, 
						kCCFThreadContextClass *ctx, ADDRINT reg_sp, 
						kCCFNode *treeTop, kCCFNode *treeBottom);

#endif