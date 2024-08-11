#include "EngineCommon/precomp.h"

#include "HAL/GameBase.h"

#include "EngineUtils/ResourceManagement/ResourceManager.h"

namespace HAL
{
	GameBase::GameBase(Engine* engine, ResourceManager& resourceManager)
		: mEngine(engine)
		, mResourceManager(resourceManager)
	{
	}

	GameBase::~GameBase() = default;

	Engine* GameBase::getEngine()
	{
		return mEngine;
	}

	ResourceManager& GameBase::getResourceManager()
	{
		return mResourceManager;
	}
} // namespace HAL
