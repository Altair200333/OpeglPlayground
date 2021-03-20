#pragma once
#include <chrono>
#include <vector>
#include <numeric>

class FPSCounter final
{
	 
public:
	static FPSCounter& instance()
	{
		static FPSCounter counter;
		return counter;
	}
	
	std::chrono::time_point<std::chrono::system_clock> lastFrameTime;

	std::vector<float> times;

	float lastFPS = 0;
	float elapsedFrameTime = 0;
	float frameTime = 0;

	static void updateFps()
	{
		auto& inst = instance();
		const auto current = std::chrono::system_clock::now();
		const std::chrono::duration<float> elapsedSeconds = current - inst.lastFrameTime;

		inst.lastFrameTime = current;
		inst.frameTime = elapsedSeconds.count();

		inst.times.push_back(inst.frameTime);

		inst.elapsedFrameTime += inst.frameTime;
		if (inst.elapsedFrameTime >= 0.5)
		{
			inst.elapsedFrameTime = 0;
			inst.lastFPS = std::accumulate(inst.times.begin(), inst.times.end(), 0.0) / static_cast<float>(inst.times.size());
			inst.times.clear();
		}
	}
	static float getFrameTime()
	{
		return instance().frameTime;
	}
	static float getFPS()
	{
		return instance().lastFPS;
	}
};
