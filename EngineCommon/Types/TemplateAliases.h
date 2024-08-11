#pragma once

#include <tuple>
#include <vector>

template<typename... Values>
using TupleVector = std::vector<std::tuple<Values...>>;

template<class T>
using RawPtr = T*;
