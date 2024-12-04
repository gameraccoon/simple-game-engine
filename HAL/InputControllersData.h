#pragma once

#include "EngineData/Input/PlayerControllerStates.h"

namespace HAL
{
	class InputControllersData
	{
	public:
		void resetLastFrameStates()
		{
			controllerStates.clearLastFrameState();
		}

	public:
		Input::PlayerControllerStates controllerStates;
	};
}
