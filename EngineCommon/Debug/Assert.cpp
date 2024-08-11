#include "EngineCommon/precomp.h"

#include "EngineCommon/Debug/Assert.h"

#include <exception>

#include <nemequ/debug-trap.h>

AssertHandlerFn gGlobalAssertHandler = [] { psnip_trap(); };

AssertHandlerFn gGlobalFatalAssertHandler = [] { std::terminate(); };

bool gGlobalAllowAssertLogs = true;
