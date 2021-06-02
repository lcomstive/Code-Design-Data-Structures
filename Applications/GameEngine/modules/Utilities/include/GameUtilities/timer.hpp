#pragma once
#include <chrono>
#include <functional>
#include <GameUtilities/messagebus.hpp>

namespace Utilities
{
	class Timer
	{
	public:
		bool EventOnStopped = false;

		Timer(const char* name, bool autostart = true);
		Timer(std::string name = "Timer", bool autostart = true);
		Timer(const Timer& timer);
		~Timer();

		void Start();
		void Stop();
		void AddStoppedCallback(std::function<void(Timer)> callback);

		std::chrono::microseconds elapsedTime();
		std::chrono::milliseconds elapsedTimeMS();

		std::string name();
		bool stopped();
		std::chrono::time_point<std::chrono::high_resolution_clock> endTime();
		std::chrono::time_point<std::chrono::high_resolution_clock> startTime();

	private:
		bool m_Stopped;
		std::string m_Name;
		std::chrono::microseconds m_ElapsedTime;
		std::vector<std::function<void(Timer)>> m_Callbacks;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_EndTime;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
	};
}