// Reference to TheCherno's Hazel Instrumentor
//// https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Debug/Instrumentor.h

#include <sstream>
#include <iomanip>
#include <GameUtilities/log.hpp>
#include <GameUtilities/file_io.hpp>
#include <GameUtilities/inline-profiler.hpp>
#include <utility>

using namespace std;
using namespace chrono;
using namespace Utilities;

mutex InlineProfiler::m_FileMutex;
string InlineProfiler::m_FilePath = "";
stringstream InlineProfiler::m_WriteStream;
unsigned int InlineProfiler::m_WriteCount = 0;

void InlineProfiler::Start(string filepath)
{
	GAME_LOG_ASSERT(m_FilePath.empty(), "InlineProfiler::Start but previous session wasn't ended");

	m_FilePath = std::move(filepath);
	WriteHeader();
}

void InlineProfiler::End()
{
	if (m_FilePath == "")
		return;

	WriteFooter();

	m_FilePath = "";
}

void InlineProfiler::WriteHeader()
{
	m_WriteCount = 0;
	m_WriteStream = stringstream();
	m_WriteStream << std::setprecision(3) << fixed;

	lock_guard guard(m_FileMutex);
	ofstream outputFile(m_FilePath, ios::trunc);
	if (outputFile.is_open())
		outputFile << "{ \"displayTimeUnits\": \"ns\", \"traceEvents\": [{}";
}

void InlineProfiler::WriteFooter()
{
	m_WriteStream << " ]}";

	lock_guard guard(m_FileMutex);
	ofstream outputFile(m_FilePath, ios::app);
	if (outputFile.is_open())
		outputFile << m_WriteStream.str();
}

void InlineProfiler::EndScope(InlineProfileScope& scope)
{
	scope.Timer.Stop();
	scope.End();

	bool defaultFile = m_FilePath.empty();
	if (m_FilePath.empty())
	{
		GAME_LOG_WARNING("No InlineProfiler session was started, overwriting default file...");
		InlineProfiler::Start();
	}

	m_WriteStream << ", { ";
	m_WriteStream << "\"name\": \"" << scope.Name << "\", ";
	m_WriteStream << "\"cat\": \"PERF\", ";
	m_WriteStream << "\"ph\": \"X\", ";
	m_WriteStream << "\"ts\": " << duration_cast<microseconds>(scope.Timer.startTime().time_since_epoch()).count() << ", ";
	m_WriteStream << "\"dur\": " << scope.Timer.elapsedTime().count() << ", ";
	m_WriteStream << "\"pid\": 0, ";
	m_WriteStream << "\"tid\": " << scope.Thread;
	m_WriteStream << " }";

	if (++m_WriteCount > WriteCountLimit)
	{
		lock_guard guard(m_FileMutex);
		ofstream outputFile(m_FilePath, ios::app);
		if (outputFile.is_open())
			outputFile << m_WriteStream.str();

		m_WriteCount = 0;
		m_WriteStream.str(string());
	}

	if (defaultFile)
		InlineProfiler::End();
}