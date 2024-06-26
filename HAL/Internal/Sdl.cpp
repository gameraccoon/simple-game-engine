#include "EngineCommon/precomp.h"

#ifndef DISABLE_SDL

#include "Sdl.h"

#include <SDL.h>
#include <stdexcept>

namespace HAL
{
	namespace Internal
	{
		SDLInstance::SDLInstance(const unsigned int flags)
		{
			if (SDL_Init(flags) != 0)
			{
				throw std::runtime_error("Failed to init SDL");
			}
		}

		SDLInstance::~SDLInstance()
		{
			SDL_Quit();
		}
	}
}

#endif // !DISABLE_SDL
