#include "EngineCommon/precomp.h"

#include "EngineData/Input/InputBindingTypes.h"

#include <algorithm>

#include "EngineData/Input/PlayerControllerStates.h"

namespace Input
{
	PressSingleButtonKeyBinding::PressSingleButtonKeyBinding(const ControllerType controllerType, const int button)
		: mControllerType(controllerType)
		, mButton(button)
	{
		AssertFatal(mControllerType < ControllerType::Count, "Invalid controller type specified");
	}

	GameplayInput::KeyState PressSingleButtonKeyBinding::getState(const PlayerControllerStates& controllerStates) const
	{
		using namespace GameplayInput;

		const auto& state = controllerStates.getState(mControllerType);

		KeyState result = KeyState::Inactive;

		if (state.isButtonPressed(mButton))
		{
			result = KeyState::Active;
		}

		if (state.isButtonJustPressed(mButton))
		{
			result = MergeKeyStates(result, KeyState::JustActivated);
		}

		if (state.isButtonJustReleased(mButton))
		{
			result = MergeKeyStates(result, KeyState::JustDeactivated);
		}

		return result;
	}

	PressButtonChordKeyBinding::PressButtonChordKeyBinding(const ControllerType controllerType, const std::vector<int>& buttons)
		: mControllerType(controllerType)
		, mButtons(buttons)
	{}

	GameplayInput::KeyState PressButtonChordKeyBinding::getState(const PlayerControllerStates& controllerStates) const
	{
		using namespace GameplayInput;

		if (mButtons.empty())
		{
			return KeyState::Inactive;
		}

		const auto& state = controllerStates.getState(mControllerType);

		const int countTotal = static_cast<int>(mButtons.size());
		int countPressed = 0;
		// note that some keys can be justPressed and justReleased during the same frame
		int countJustPressed = 0;
		int countJustReleased = 0;
		int flipFloppedDownUp = 0;

		for (const int button : mButtons)
		{
			// none of these three are exclusive
			const bool isPressed = state.isButtonPressed(button);
			const bool isJustPressed = state.isButtonJustPressed(button);
			const bool isJustReleased = state.isButtonJustReleased(button);

			// to manage special cases when one button changed state twice in one frame
			const bool flipFlopDownUp = isJustPressed && isJustReleased && !isPressed;

			countPressed += isPressed ? 1 : 0;
			countJustPressed += isJustPressed ? 1 : 0;
			countJustReleased += isJustReleased ? 1 : 0;
			flipFloppedDownUp += flipFlopDownUp ? 1 : 0;
		}

		KeyState result = KeyState::Inactive;
		if (countPressed == countTotal)
		{
			result = KeyState::Active;
		}

		if (countJustPressed > 0 && countPressed + flipFloppedDownUp == countTotal)
		{
			result = MergeKeyStates(result, KeyState::JustActivated);
		}

		if (countPressed - countJustPressed + countJustReleased + flipFloppedDownUp == countTotal)
		{
			result = MergeKeyStates(result, KeyState::JustDeactivated);
		}

		return result;
	}

	PositiveButtonAxisBinding::PositiveButtonAxisBinding(const ControllerType controllerType, const int button)
		: mControllerType(controllerType)
		, mButton(button)
	{}

	float PositiveButtonAxisBinding::getAxisValue(const PlayerControllerStates& controllerStates) const
	{
		const auto& state = controllerStates.getState(mControllerType);
		return state.isButtonPressed(mButton) ? 1.0f : 0.0f;
	}

	NegativeButtonAxisBinding::NegativeButtonAxisBinding(const ControllerType controllerType, const int button)
		: mControllerType(controllerType)
		, mButton(button)
	{}

	float NegativeButtonAxisBinding::getAxisValue(const PlayerControllerStates& controllerStates) const
	{
		const auto& state = controllerStates.getState(mControllerType);
		return state.isButtonPressed(mButton) ? -1.0f : 0.0f;
	}

	DirectAxisToAxisBinding::DirectAxisToAxisBinding(const ControllerType controllerType, const int axis)
		: mControllerType(controllerType)
		, mAxis(axis)
	{}

	float DirectAxisToAxisBinding::getAxisValue(const PlayerControllerStates& controllerStates) const
	{
		const auto& state = controllerStates.getState(mControllerType);
		return state.getAxisValue(mAxis);
	}
} // namespace Input
