#pragma once
#include <map>
#include <string>
#include <thread>
#include <fstream>
#include <GameUtilities/log.hpp>
#include <GameUtilities/timer.hpp>

using namespace std;

namespace Utilities
{
	class InlineProfiler
	{
	private:
		static string m_FilePath;
		static mutex m_FileMutex;
		static unsigned int m_WriteCount;
		static stringstream m_WriteStream;

		static const unsigned int WriteCountLimit = 500;

		static void WriteHeader();
		static void WriteFooter();

	public:
		struct InlineProfileScope
		{
			bool Debug;
			Timer Timer;
			string Name;
			thread::id Thread;

			InlineProfileScope(string name, bool debug = false) : Timer(name), Name(name), Debug(debug)
			{
				Thread = this_thread::get_id();
				if(Debug)
					GAME_LOG_DEBUG("[PROFILE] Start '" + Name + "'");
			}

			InlineProfileScope(Utilities::Timer& timer, bool debug = false) : Timer(timer), Name(timer.name()), Debug(debug)
			{
				Thread = this_thread::get_id();
				if (Debug)
					GAME_LOG_DEBUG("[PROFILE] Start '" + Timer.name() + "'");
			}
			InlineProfileScope(const InlineProfileScope& scope) : Timer(scope.Timer), Name(scope.Name), Thread(scope.Thread), Debug(scope.Debug) { }
			~InlineProfileScope() { End(); }
			
			void End()
			{
				if (Timer.stopped())
					return;
				Timer.Stop();
				if(Debug)
					GAME_LOG_DEBUG("[PROFILE] End '" + Name + "' - " + to_string(Timer.elapsedTimeMS().count()) + "ms");
				InlineProfiler::EndScope(*this);
			}
		};

		static void Start(string filepath = "profile.json");
		static void End();

		static void EndScope(InlineProfileScope& session);
	};
}

#ifndef NDEBUG
#define MAD_PROFILE 1
#else
#define MAD_PROFILE 0
#endif
	
#if MAD_PROFILE
	#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
		#define MAD_FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(__DMC__) && (__DMC__ >= 0x810)
		#define MAD_FUNC_SIG __PRETTY_FUNCTION__
	#elif (defined(__FUNCSIG__) || (_MSC_VER))
		#define MAD_FUNC_SIG __FUNCSIG__
	#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
		#define MAD_FUNC_SIG __FUNCTION__
	#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
		#define MAD_FUNC_SIG __FUNC__
	#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
		#define MAD_FUNC_SIG __func__
	#elif defined(__cplusplus) && (__cplusplus >= 201103)
		#define MAD_FUNC_SIG __func__
	#else
		#define MAD_FUNC_SIG "MAD_FUNC_SIG unknown!"
	#endif

	#define PROFILE_BEGIN(filename) Utilities::InlineProfiler::Start(filename)
	#define PROFILE_END() Utilities::InlineProfiler::End()
	#define PROFILE_SCOPE_LINE2(name, line) auto scope##line = Utilities::InlineProfiler::InlineProfileScope(string(name));
	#define PROFILE_SCOPE_LINE(name, line) PROFILE_SCOPE_LINE2(name, line)
	#define PROFILE_SCOPE(name) PROFILE_SCOPE_LINE(name, __LINE__)
	#define PROFILE_FN() PROFILE_SCOPE(MAD_FUNC_SIG)

	#define PROFILE_SCOPE_LINE2_DEBUG(name, line) auto scope##line = Utilities::InlineProfiler::InlineProfileScope(string(name), true);
	#define PROFILE_SCOPE_LINE_DEBUG(name, line) PROFILE_SCOPE_LINE2_DEBUG(name, line)
	#define PROFILE_SCOPE_DEBUG(name) PROFILE_SCOPE_LINE_DEBUG(name, __LINE__)
	#define PROFILE_FN_DEBUG() PROFILE_SCOPE_DEBUG(MAD_FUNC_SIG)
#else
	#define PROFILE_BEGIN(filename)
	#define PROFILE_END()
	#define PROFILE_SCOPE_LINE(name, line)
	#define PROFILE_SCOPE(name)
	#define PROFILE_FN()

	#define PROFILE_SCOPE_LINE2_DEBUG(name, line)
	#define PROFILE_SCOPE_LINE_DEBUG(name, line)
	#define PROFILE_SCOPE_DEBUG(name)
	#define PROFILE_FN_DEBUG()
#endif