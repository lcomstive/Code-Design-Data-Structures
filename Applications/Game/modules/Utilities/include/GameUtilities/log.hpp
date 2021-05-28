#pragma once

#pragma warning(disable : 4996)

#include <mutex>
#include <string>
#include <GameUtilities/messagebus.hpp>

#define GAME_LOG_INFO		Utilities::Log::Info
#define GAME_LOG_WARNING	Utilities::Log::Warning
#define GAME_LOG_ERROR		Utilities::Log::Error

// #ifndef NDEBUG
#define GAME_LOG_ENABLE_DEBUGGING
// #endif

#ifdef GAME_LOG_ENABLE_DEBUGGING
#define GAME_LOG_DEBUG	Utilities::Log::Debug
#define GAME_LOG_ASSERT(x, msg) if(!(x)) { GAME_LOG_ERROR(msg); throw runtime_error(msg); }
#define GAME_LOG_ASSERT_RESULT(x, msg) if(x) { GAME_LOG_ERROR("[" + to_string(x) + "] " + msg); throw runtime_error(msg); }
#else
#define GAME_LOG_DEBUG
#define GAME_LOG_ASSERT(x, msg)
#define GAME_LOG_ASSERT_RESULT(x, msg) x;
#endif

using namespace std;
using namespace std::filesystem;

namespace Utilities
{
	class Log
	{
	public:
		static const string& LogFilePath;
		static const string& ErrorFilePath;

		// Prevent any non-static versions of this class
		Log() = delete;
		~Log() = delete;

		static void Initialize(bool outputToConsole = true);

		inline static void Error(string message) { MessageBus::eventBus()->Send("LogError", DataStream().write<string>(message)); }

		inline static void Info(string message = "") { MessageBus::eventBus()->Send("LogInfo", DataStream().write<string>(message)); }
		inline static void Debug(string message) { MessageBus::eventBus()->Send("LogDebug", DataStream().write<string>(message)); }
		inline static void Warning(string message) { MessageBus::eventBus()->Send("LogWarning", DataStream().write<string>(message)); }

	private:
		static atomic_bool m_Initialized;

		static mutex m_LogMutex;
		static mutex m_ErrorMutex;

		static void WriteToFile(string text, bool error = false);
	};
}