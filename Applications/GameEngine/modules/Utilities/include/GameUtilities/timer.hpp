#pragma once
#include <chrono>
#include <functional>
#include <GameUtilities/messagebus.hpp>

using namespace std;
using namespace chrono;

namespace Utilities
{
	class Timer
	{
	public:
		bool EventOnStopped = false;

		Timer(const char* name, bool autostart = true);
		Timer(string name = "Timer", bool autostart = true);
		Timer(const Timer& timer);
		~Timer();

		void Start();
		void Stop();
		void AddStoppedCallback(function<void(Timer)> callback);

		microseconds elapsedTime();
		milliseconds elapsedTimeMS();

		string name();
		bool stopped();
		time_point<high_resolution_clock> endTime();
		time_point<high_resolution_clock> startTime();

	private:
		bool m_Stopped;
		string m_Name;
		microseconds m_ElapsedTime;
		vector<function<void(Timer)>> m_Callbacks;
		time_point<high_resolution_clock> m_EndTime;
		time_point<high_resolution_clock> m_StartTime;
	};
}