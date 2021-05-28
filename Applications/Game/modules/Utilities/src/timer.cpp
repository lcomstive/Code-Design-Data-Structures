#include <GameUtilities/timer.hpp>
#include <GameUtilities/inline-profiler.hpp>

using namespace std;
using namespace chrono;
using namespace Utilities;

Timer::Timer(const char* name, bool autostart) : Timer(string(name)) { }
Timer::Timer(string name, bool autostart) : m_Name(name), m_Stopped(false), m_ElapsedTime(0) { if(autostart) Start(); }
Timer::~Timer() { if (!m_Stopped) Stop(); }

Timer::Timer(const Timer& timer) :
	m_Name(timer.m_Name),
	m_Stopped(timer.m_Stopped),
	m_StartTime(timer.m_StartTime),
	m_EndTime(timer.m_EndTime),
	m_ElapsedTime(timer.m_ElapsedTime)
	{ m_Callbacks.insert(m_Callbacks.end(), timer.m_Callbacks.begin(), timer.m_Callbacks.end()); }

void Timer::Start()
{
	m_Stopped = false;
	m_StartTime = high_resolution_clock::now();
}

void Timer::Stop()
{
	if (m_Stopped)
		return;
	m_EndTime = high_resolution_clock::now();
	m_ElapsedTime = duration_cast<microseconds>(m_EndTime - m_StartTime);
	m_Stopped = true;

	for (size_t i = 0; i < m_Callbacks.size(); i++)
		m_Callbacks[i](*this);
	if (EventOnStopped) MessageBus::eventBus()->Send("TimerStopped", DataStream().write(m_Name)->write(m_ElapsedTime));
}

void Timer::AddStoppedCallback(function<void(Timer)> callback) { m_Callbacks.push_back(callback); }

microseconds Timer::elapsedTime()
{
	if (m_Stopped)
		return m_ElapsedTime;
	return duration_cast<microseconds>(high_resolution_clock::now() - m_StartTime);
}

milliseconds Timer::elapsedTimeMS() { return duration_cast<milliseconds>(elapsedTime()); }

string Timer::name() { return m_Name; }
bool Timer::stopped() { return m_Stopped; }
time_point<high_resolution_clock> Timer::startTime() { return m_StartTime; }
time_point<high_resolution_clock> Timer::endTime() { return m_EndTime; }
