#pragma once

namespace GameplayInput
{
	enum class KeyState
	{
		Inactive = 0,
		JustActivated = 1 << 0,
		JustDeactivated = 1 << 1,
		Active = 1 << 2,
	};

	KeyState MergeKeyStates(auto... state) noexcept
	{
		return static_cast<KeyState>((... | static_cast<int>(state)));
	}

	bool AreStatesIntersecting(KeyState state1, KeyState state2) noexcept;
} // namespace GameplayInput
