#pragma once

#include <thread>

#include "EngineCommon/TimeConstants.h"

#include "HAL/IGame.h"

namespace HAL
{
	template<typename ShouldStopFnT = std::nullptr_t, typename OnIterationFnT = std::nullptr_t, typename AfterFrameFnT = std::nullptr_t>
	void RunGameLoop(IGame& game, ShouldStopFnT&& shouldStopFn = nullptr, OnIterationFnT&& onIterationFn = nullptr, AfterFrameFnT&& afterFrameFn = nullptr)
	{
		constexpr auto oneFrameDuration = TimeConstants::ONE_FIXED_UPDATE_DURATION;

		auto lastFrameTime = std::chrono::steady_clock::now() - oneFrameDuration;

		while (!game.shouldQuitGame())
		{
			if constexpr (!std::is_same_v<ShouldStopFnT, std::nullptr_t>)
			{
				 if (shouldStopFn())
				 {
					 break;
				 }
			}

			if constexpr (!std::is_same_v<OnIterationFnT, std::nullptr_t>)
			{
				onIterationFn();
			}

			auto timeNow = std::chrono::steady_clock::now();

			int iterationsThisFrame = 0;
			auto passedTime = timeNow - lastFrameTime;

			const auto correctedOneFrameDuration = oneFrameDuration + game.getFrameLengthCorrection();
			if (passedTime >= correctedOneFrameDuration)
			{
				// if we exceeded max frame ticks last frame, that likely mean we were staying on a breakpoint
				// so, let's readjust to normal ticking speed
				if (passedTime > TimeConstants::MAX_FRAME_DURATION)
				{
					passedTime = correctedOneFrameDuration;
				}

				const float lastFrameDurationSec = std::chrono::duration<float>(passedTime).count();

				while (passedTime >= correctedOneFrameDuration)
				{
					passedTime -= correctedOneFrameDuration;
					++iterationsThisFrame;
				}

				game.notPausablePreFrameUpdate(lastFrameDurationSec);

				if (!game.shouldPauseGame())
				{
					game.dynamicTimePreFrameUpdate(lastFrameDurationSec, iterationsThisFrame);
					for (int i = 0; i < iterationsThisFrame; ++i)
					{
						game.fixedTimeUpdate(TimeConstants::ONE_FIXED_UPDATE_SEC);
					}
					game.dynamicTimePostFrameUpdate(lastFrameDurationSec, iterationsThisFrame);
				}

				game.notPausablePostFrameUpdate(lastFrameDurationSec);

				lastFrameTime = timeNow - passedTime;

				if constexpr (!std::is_same_v<AfterFrameFnT, std::nullptr_t>)
				{
					afterFrameFn();
				}
			}

			if (iterationsThisFrame <= 1)
			{
				std::this_thread::yield();
			}
		}
	}
}
