#pragma once

#include <vector>

#include "EngineData/Input/ControllerType.h"
#include "EngineData/Input/GameplayInputKeyState.h"
#include "EngineData/Input/InputBindingBase.h"

namespace Input
{
	struct PlayerControllerStates;

	/**
	 * returns true if the button is pressed
	 */
	class PressSingleButtonKeyBinding final : public KeyBinding
	{
	public:
		PressSingleButtonKeyBinding(ControllerType controllerType, int button);
		GameplayInput::KeyState getState(const PlayerControllerStates& controllerStates) const override;

	private:
		const ControllerType mControllerType;
		const int mButton;
	};

	/**
	 * Processes a chord of buttons (can be one or zero button as well)
	 * returns false if expected chord is empty
	 * returns false if any of buttons in the chord are not pressed
	 * otherwice returns true
	 */
	class PressButtonChordKeyBinding final : public KeyBinding
	{
	public:
		PressButtonChordKeyBinding(ControllerType controllerType, const std::vector<int>& buttons);
		GameplayInput::KeyState getState(const PlayerControllerStates& controllerStates) const override;

	private:
		const ControllerType mControllerType;
		const std::vector<int> mButtons;
	};

	/**
	 * returns 1.0 when button is pressed, and 0.0 when it's released
	 */
	class PositiveButtonAxisBinding final : public AxisBinding
	{
	public:
		PositiveButtonAxisBinding(ControllerType controllerType, int button);
		virtual float getAxisValue(const PlayerControllerStates& controllerStates) const override;

	private:
		const ControllerType mControllerType;
		const int mButton;
	};

	/**
	 * returns -1.0 when button is pressed, and 0.0 when it's released
	 */
	class NegativeButtonAxisBinding final : public AxisBinding
	{
	public:
		NegativeButtonAxisBinding(ControllerType controllerType, int button);
		virtual float getAxisValue(const PlayerControllerStates& controllerStates) const override;

	private:
		const ControllerType mControllerType;
		const int mButton;
	};

	/**
	 * returns unchanged value of the controller axis
	 */
	class DirectAxisToAxisBinding final : public AxisBinding
	{
	public:
		DirectAxisToAxisBinding(ControllerType controllerType, int axis);
		virtual float getAxisValue(const PlayerControllerStates& controllerStates) const override;

	private:
		const ControllerType mControllerType;
		const int mAxis;
	};
} // namespace Input
