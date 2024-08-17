#pragma once

#ifndef DISABLE_SDL

#include <memory>
#include <thread>

#include "EngineLogic/Render/RenderAccessor.h"

class ResourceManager;

namespace HAL
{
	class Engine;
}

class RenderThreadManager
{
public:
	RenderThreadManager() = default;
	~RenderThreadManager();
	RenderThreadManager(RenderThreadManager&) = delete;
	RenderThreadManager& operator=(RenderThreadManager&) = delete;
	RenderThreadManager(RenderThreadManager&&) = delete;
	RenderThreadManager& operator=(RenderThreadManager&&) = delete;

	RenderAccessor& getAccessor() { return mRenderAccessor; }

	void startThread(ResourceManager& resourceManager, HAL::Engine& engine, std::function<void()>&& threadInitializeFn);
	void shutdownThread();

	void setAmountOfRenderedGameInstances(int instancesCount);

	/**
	 * Run rendering in the main thread.
	 * For this to work we need to make sure we don't call `startThread` or `shutdownThread`
	 */
	void runInMainThread(RenderAccessor& renderAccessor, ResourceManager& resourceManager, HAL::Engine& engine);

private:
	static void RenderThreadFunction(RenderAccessor& renderAccessor, ResourceManager& resourceManager, HAL::Engine& engine);

private:
	// contains everything needed to communicate with the render thread
	RenderAccessor mRenderAccessor;
	std::unique_ptr<std::thread> mRenderThread;
};

#endif // DISABLE_SDL
