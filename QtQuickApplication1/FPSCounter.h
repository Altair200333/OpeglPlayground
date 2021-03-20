#pragma once
#include <chrono>
#include <vector>
#include <numeric>

class FPSCounter final
{
public:
	std::chrono::time_point<std::chrono::system_clock> lastFrameTime;

	std::vector<float> times;

	float lastFPS = 0;
	float elapsedFrameTime = 0;
	float frameTime = 0;
	float getFPS()
	{
		
		const auto current = std::chrono::system_clock::now();
		const std::chrono::duration<float> elapsedSeconds = current - lastFrameTime;
		lastFrameTime = current;
		frameTime = elapsedSeconds.count();
		
		times.push_back(frameTime);
		
		elapsedFrameTime += frameTime;
		if(elapsedFrameTime >= 0.5)
		{
			elapsedFrameTime = 0;
			lastFPS = std::accumulate(times.begin(), times.end(), 0.0) / static_cast<float>(times.size());
			times.clear();
		}
		return lastFPS;
	}
};
