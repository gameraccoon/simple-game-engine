#include "EngineCommon/precomp.h"

#include "EngineData/Input/GameplayInputKeyState.h"

namespace GameplayInput
{
	bool AreStatesIntersecting(KeyState state1, KeyState state2) noexcept
	{
		return (static_cast<int>(state1) & static_cast<int>(state2)) != 0;
	}
} // namespace GameplayInput
