#pragma once

#include <iomanip>
#include "util.h"

VOID Fini(INT32 code, VOID *v);
void funcTraceDebugDump(GlobalContext *globalCtx, FunctionObj *fc,
                        ThreadContext *ctx, ADDRINT reg_sp,
                        ihppNode *treeTop, ihppNode *treeBottom);
