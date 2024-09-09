#pragma once

#include <optional>
#include <vector>

#include "EngineCommon/Types/String/StringId.h"

template<typename T>
StringId get_enum_name();

template<typename T>
StringId enum_to_string(T value);

template<typename T>
std::optional<T> string_to_enum(StringId value);

template<typename T>
std::vector<T> get_all_enum_values();

template<typename T>
std::vector<StringId> get_all_enum_value_names();
