#pragma once

#include "EngineData/Input/GameplayInputKeyState.h"

namespace Input
{
	struct PlayerControllerStates;

	class KeyBinding
	{
	public:
		virtual ~KeyBinding() = default;
		virtual GameplayInput::KeyState getState(const PlayerControllerStates& controllerStates) const = 0;
	};

	class AxisBinding
	{
	public:
		virtual ~AxisBinding() = default;
		virtual float getAxisValue(const PlayerControllerStates& controllerStates) const = 0;
	};
} // namespace Input
