#include "EngineCommon/precomp.h"

#ifndef DISABLE_SDL

#include <SDL_mixer.h>

#include "HAL/Audio/Music.h"
#include "HAL/Base/Engine.h"

namespace Audio
{
	Music::Music(const AbsoluteResourcePath& path)
		: mMusic(Mix_LoadMUS(path.getAbsolutePathStr().c_str()))
	{
	}

	Music::~Music()
	{
		Mix_FreeMusic(mMusic);
	}

	Mix_Music* Music::getRawMusic() const
	{
		return mMusic;
	}

	bool Music::isValid() const
	{
		return mMusic != nullptr;
	}
} // namespace Audio

#endif // !DISABLE_SDL
