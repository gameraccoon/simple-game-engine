#include "EngineCommon/precomp.h"

#include "EngineCommon/Types/String/ResourcePath.h"

#include <nlohmann/json.hpp>

void to_json(nlohmann::json& outJson, const RelativeResourcePath& path)
{
	outJson = nlohmann::json(path.getRelativePathStr());
}

void from_json(const nlohmann::json& json, RelativeResourcePath& path)
{
	path = RelativeResourcePath(json.get<std::string>());
}
