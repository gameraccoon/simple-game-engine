#pragma once

#include "EngineCommon/Debug/LogCategory.h"

constexpr LogCategory LOG_LOGGING{ "LOGGING" };
constexpr LogCategory LOG_ASSERT{ "ASSERT" };
constexpr LogCategory LOG_RENDERING{ "RENDERING" };
constexpr LogCategory LOG_RESOURCE_LOADING{ "RESOURCE_LOADING" };
constexpr LogCategory LOG_NETWORK{ "NETWORK" };
// For debugging only, don't commit these
constexpr LogCategory LOG_LOCAL_DEBUGGING{ "LOCAL_DEBUGGING" };
