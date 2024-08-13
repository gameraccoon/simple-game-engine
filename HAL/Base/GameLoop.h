#pragma once

#include <thread>

#include "EngineCommon/TimeConstants.h"

#include "HAL/IGame.h"

namespace HAL
{
	// Why we need a class for a game loop? For emscripten we have to give control back to the
	// browser code after each loop so we have to store state somewhere outside of the function
	// And it is better not to duplicate the code for different platforms
	class GameLoopRunner
	{
	public:
		GameLoopRunner() noexcept
			: mLastFrameTime(std::chrono::steady_clock::now() - TimeConstants::ONE_FIXED_UPDATE_DURATION)
		{}

		template<typename AfterFrameFnT = std::nullptr_t>
		void loopBodyFn(IGame& game, AfterFrameFnT&& afterFrameFn = nullptr) noexcept
		{
			const auto timeNow = std::chrono::steady_clock::now();

			mIterationsThisFrame = 0;
			auto passedTime = timeNow - mLastFrameTime;

			const auto correctedOneFrameDuration = TimeConstants::ONE_FIXED_UPDATE_DURATION + game.getFrameLengthCorrection();
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
					++mIterationsThisFrame;
				}

				game.notPausablePreFrameUpdate(lastFrameDurationSec);

				if (!game.shouldPauseGame())
				{
					game.dynamicTimePreFrameUpdate(lastFrameDurationSec, mIterationsThisFrame);
					for (int i = 0; i < mIterationsThisFrame; ++i)
					{
						game.fixedTimeUpdate(TimeConstants::ONE_FIXED_UPDATE_SEC);
					}
					game.dynamicTimePostFrameUpdate(lastFrameDurationSec, mIterationsThisFrame);
				}

				game.notPausablePostFrameUpdate(lastFrameDurationSec);

				mLastFrameTime = timeNow - passedTime;

				if constexpr (!std::is_same_v<AfterFrameFnT, std::nullptr_t>)
				{
					afterFrameFn();
				}
			}
		}

		template<typename ShouldStopFnT = std::nullptr_t, typename OnIterationFnT = std::nullptr_t, typename AfterFrameFnT = std::nullptr_t>
		void run(IGame& game, ShouldStopFnT&& shouldStopFn = nullptr, OnIterationFnT&& onIterationFn = nullptr, AfterFrameFnT&& afterFrameFn = nullptr) noexcept
		{
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

				loopBodyFn(
					game,
					std::forward<AfterFrameFnT>(afterFrameFn)
				);

				if (mIterationsThisFrame <= 1)
				{
					std::this_thread::yield();
				}
			}
		}

	private:
		std::chrono::time_point<std::chrono::steady_clock> mLastFrameTime;
		int mIterationsThisFrame = 0;
	};

	template<typename ShouldStopFnT = std::nullptr_t, typename OnIterationFnT = std::nullptr_t, typename AfterFrameFnT = std::nullptr_t>
	void RunGameLoop(IGame& game, ShouldStopFnT&& shouldStopFn = nullptr, OnIterationFnT&& onIterationFn = nullptr, AfterFrameFnT&& afterFrameFn = nullptr) noexcept
	{
		GameLoopRunner loopRunner;
		loopRunner.run(game, std::move(shouldStopFn), std::move(onIterationFn), std::move(afterFrameFn));
	}
} // namespace HAL
