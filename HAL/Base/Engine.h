#pragma once

#include <memory>

#include "EngineData/Geometry/Vector2D.h"

#include "HAL/InputControllersData.h"

struct SDL_Window;
union SDL_Event;

namespace HAL
{
	class IGame;

#ifndef DISABLE_SDL
	class Engine
	{
	public:
		Engine(int windowWidth, int windowHeight) noexcept;

		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;
		Engine(Engine&&) = delete;
		Engine& operator=(Engine&&) = delete;

		~Engine();

		void init(IGame* game, InputControllersData* inputControllersData);
		void start();

		Vector2D getWindowSize() const;

		void releaseRenderContext();
		void acquireRenderContext();

		// for debug tools such as imgui
		SDL_Window* getRawWindow();
		void* getRawGlContext();

		// these are manual functions that should be called by the game loop
		void parseEvents();
		std::vector<SDL_Event>& getLastFrameEvents();

	private:
		struct Impl;
		std::unique_ptr<Impl> mPimpl;
	};
#endif // !DISABLE_SDL
} // namespace HAL
