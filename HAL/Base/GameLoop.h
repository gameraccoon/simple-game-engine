#pragma once

#include <thread>

#include "EngineCommon/TimeConstants.h"

#include "HAL/IGame.h"

namespace HAL
{
	class GameLoopRunner
	{
	public:
		GameLoopRunner() noexcept
			: mLastRenderTime(std::chrono::steady_clock::now() - mMinRenderFrameDuration)
			, mLastFixedUpdateTime(std::chrono::steady_clock::now() - TimeConstants::ONE_FIXED_UPDATE_DURATION)
		{}

		template<typename AfterFrameFnT = std::nullptr_t>
		void loopBodyFn(IGame& game, AfterFrameFnT&& afterFrameFn = nullptr) noexcept
		{
			const auto timeBeforeUpdate = std::chrono::steady_clock::now();

			mUpdatesThisFrame = 0;
			auto passedTimeFromLastUpdate = timeBeforeUpdate - mLastFixedUpdateTime;

			const auto correctedOneUpdateDuration = TimeConstants::ONE_FIXED_UPDATE_DURATION + game.getFrameLengthCorrection();

			const bool shouldUpdateSimulation = passedTimeFromLastUpdate >= correctedOneUpdateDuration;

			if (shouldUpdateSimulation)
			{
				// if we exceeded max frame ticks last frame, that likely means we were staying on a breakpoint
				// so, let's readjust to normal ticking speed
				if (passedTimeFromLastUpdate > TimeConstants::MAX_FRAME_DURATION)
				{
					passedTimeFromLastUpdate = correctedOneUpdateDuration;
				}

				while (passedTimeFromLastUpdate >= correctedOneUpdateDuration)
				{
					passedTimeFromLastUpdate -= correctedOneUpdateDuration;
					++mUpdatesThisFrame;
				}

				const float timeFromLastUpdate = std::chrono::duration<float>(passedTimeFromLastUpdate).count();

				game.notPausablePreFrameUpdate(timeFromLastUpdate);

				if (!game.shouldPauseGame())
				{
					game.dynamicTimePreFrameUpdate(timeFromLastUpdate, mUpdatesThisFrame);
					for (int i = 0; i < mUpdatesThisFrame; ++i)
					{
						game.fixedTimeUpdate(TimeConstants::ONE_FIXED_UPDATE_SEC);
					}
					game.dynamicTimePostFrameUpdate(timeFromLastUpdate, mUpdatesThisFrame);
				}

				mLastFixedUpdateTime = timeBeforeUpdate - passedTimeFromLastUpdate;
			}

			// the update can take some time, update the timer
			const auto timeBeforeRender = shouldUpdateSimulation ? std::chrono::steady_clock::now() : timeBeforeUpdate;
			const auto passedTimeFromLastRender = timeBeforeRender - mLastRenderTime;
			const bool shouldRender = passedTimeFromLastRender >= mMinRenderFrameDuration;
			if (shouldRender)
			{
				if (!shouldUpdateSimulation)
				{
					passedTimeFromLastUpdate = timeBeforeRender - mLastFixedUpdateTime;
				}

				// at which point between updates we are (0 - just updated, 1 - about to update)
				const float frameAlpha = std::chrono::duration<float>(passedTimeFromLastUpdate).count() / std::chrono::duration<float>(correctedOneUpdateDuration).count();

				game.notPausableRenderUpdate(frameAlpha);

				mLastRenderTime = timeBeforeRender;

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

				if (mUpdatesThisFrame <= 1)
				{
					std::this_thread::yield();
				}
			}
		}

	private:
		// 140 FPS is the default frame rate cap
		std::chrono::duration<long, std::ratio<1, 1000000>> mMinRenderFrameDuration = std::chrono::microseconds{ 7142 };

		std::chrono::time_point<std::chrono::steady_clock> mLastRenderTime;
		std::chrono::time_point<std::chrono::steady_clock> mLastFixedUpdateTime;
		int mUpdatesThisFrame = 0;
	};

	template<typename ShouldStopFnT = std::nullptr_t, typename OnIterationFnT = std::nullptr_t, typename AfterFrameFnT = std::nullptr_t>
	void RunGameLoop(IGame& game, ShouldStopFnT&& shouldStopFn = nullptr, OnIterationFnT&& onIterationFn = nullptr, AfterFrameFnT&& afterFrameFn = nullptr) noexcept
	{
		GameLoopRunner loopRunner;
		loopRunner.run(game, std::move(shouldStopFn), std::move(onIterationFn), std::move(afterFrameFn));
	}
} // namespace HAL
