/*
 *
 * AIE Introduction to C++
 * Code Design & Data Structures
 * Lewis Comstive (s210314)
 *
 * See the LICENSE file in the root directory of project for copyright.
 *
 */

#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN // Try and prevent some bloat from <Windows.h>
#include <Windows.h>
#endif

#include <string>

namespace LCDS
{
	enum class ConsoleColour : unsigned int
	{
		Unchanged = 9999,

#if __unix__ || __APPLE__
		Black       = 30,
		DarkBlue    = 34,
		DarkGreen   = 32,
		DarkCyan    = 36,
		DarkRed     = 31,
		DarkMagenta = 35,
		DarkYellow  = 33,
		Grey        = 90,
		Blue        = 94,
		Green       = 92,
		Cyan        = 96,
		Red         = 91,
		Magenta     = 95,
		Yellow      = 93,
		White       = 37
#else
		Black		= 0,
		DarkBlue	= 1,
		DarkGreen	= 2,
		DarkCyan	= 3,
		DarkRed		= 4,
		DarkMagenta	= 5,
		DarkYellow	= 6,
		Grey		= 7,
		Blue		= 9,
		Green		= 10,
		Cyan		= 11,
		Red			= 12,
		Magenta		= 13,
		Yellow		= 14,
		White		= 15
#endif
	};

	struct ConsoleColourCombo
	{
		ConsoleColour foreground = ConsoleColour::White;
		ConsoleColour background = ConsoleColour::Unchanged;
	};

	/// Console controls
	class Console
	{
	public:
		static void Clear();

		static void SetTitle(const std::string& title);
		static void SetCursorPos(int x, int y);
		static void GetCursorPos(int* x, int* y); // TODO: Test this function
		static void GetScreenSize(int* width, int* height);

		static void Write(const char& letter, const ConsoleColourCombo colours = { });
		static void Write(const char& letter, const ConsoleColour foreground, const ConsoleColour background = ConsoleColour::Unchanged);
		static void WriteLine(const char& letter = '\0', const ConsoleColour foreground = ConsoleColour::White, const ConsoleColour background = ConsoleColour::Unchanged);

		static void Write(const std::string& text, const ConsoleColourCombo colours = { });
		static void Write(const std::string& text, const ConsoleColour foreground, const ConsoleColour background = ConsoleColour::Unchanged);
		static void WriteLine(const std::string& text, const ConsoleColourCombo colours = {});
		static void WriteLine(const std::string& text, const ConsoleColour foreground, const ConsoleColour background = ConsoleColour::Unchanged);

		/// Fills variables, gets handles.
		/// Also called inherently through other functions in class
		static void Initialise();
	private:
#if _WIN32
		// --- WINDOWS API --- //

		/// Stores values such as current console colour
		static WORD s_Attributes;

		/// Handle to the console process
		static HANDLE s_Handle;

		/// Retrieves information regarding the console screen's font, size and colours
		static CONSOLE_SCREEN_BUFFER_INFO GetScreenBufferInfo();
#endif
	};
}