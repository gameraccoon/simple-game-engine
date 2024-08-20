#include "EngineCommon/precomp.h"

#include "HAL/Internal/SdlWindow.h"

#ifndef DISABLE_SDL

#include <stdexcept>
#include <string>

#include <SDL.h>

namespace HAL::Internal
{
	Window::Window(const int width, const int height)
		: mSDLWindow(SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL))
	{
		if (mSDLWindow == nullptr)
		{
			throw std::runtime_error(std::string("Error creating window: ") + SDL_GetError());
		}
	}

	Window::~Window()
	{
		SDL_DestroyWindow(mSDLWindow);
	}

	void Window::show()
	{
		SDL_ShowWindow(mSDLWindow);
	}

	SDL_Window* Window::getRawWindow()
	{
		return mSDLWindow;
	}
} // namespace HAL::Internal

#endif // !DISABLE_SDL
