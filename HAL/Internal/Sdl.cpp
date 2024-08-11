#include "EngineCommon/precomp.h"

#ifndef DISABLE_SDL

#include <stdexcept>

#include "Sdl.h"
#include <SDL.h>

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
	} // namespace Internal
} // namespace HAL

#endif // !DISABLE_SDL
