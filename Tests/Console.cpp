#include <cstdio>
#include <iostream>

#include "Console.hpp"

using namespace std;
using namespace LCDS;

#ifdef _WIN32
WORD Console::s_Attributes = 0;
HANDLE Console::s_Handle = nullptr;

/// Retrieve the console screen buffer for size and colours
CONSOLE_SCREEN_BUFFER_INFO Console::GetScreenBufferInfo()
{
	CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
	GetConsoleScreenBufferInfo(s_Handle, &bufferInfo);
	return bufferInfo;
}
#elif __unix__ || __APPLE__
#include <unistd.h>		// STDOUT_FILENO
#include <sys/ioctl.h>  // ioctl() and TIOCGWINSZ
#endif

void Console::Initialise()
{
#ifdef _WIN32
	if (s_Handle != nullptr)
		return; // Already initialised

	// Get console handle
	s_Handle = GetStdHandle(STD_OUTPUT_HANDLE);

	// Grab console info (size, cursor position, font, etc.)
	s_Attributes = GetScreenBufferInfo().wAttributes;
#ifdef CONSOLE_UTF8_SUPPORT
	// Set console to UTF-8 mode
	SetConsoleOutputCP(CP_UTF8);
#endif
#endif
}

void Console::Clear()
{
	Initialise();

#ifdef _WIN32
	// Determine amount of cells to clear
	int width, height;
	Console::GetScreenSize(&width, &height);
	const int cellCount = width * height;
	DWORD count;

	// Fill the entire buffer with empty characters
	if (!FillConsoleOutputCharacter(
		s_Handle,
		' ',
		cellCount,
		COORD{ 0, 0 },
		&count
		))
		return;

	// Reset cursor
	SetCursorPos(0, 0);
#elif __unix__ || __APPLE__
	printf("\033[2J\033[1;1H");
#endif
}

void Console::GetScreenSize(int* width, int* height)
{
	Initialise();

#ifdef _WIN32
	const CONSOLE_SCREEN_BUFFER_INFO bufferInfo = GetScreenBufferInfo();
	*width = bufferInfo.dwSize.X;
	*height = bufferInfo.dwSize.Y;
#elif __unix__ || __APPLE__
	// https://stackoverflow.com/a/23369919

	struct winsize size;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	*width = size.ws_row;
	*height = size.ws_col;
#endif
}

void Console::SetCursorPos(int x, int y)
{
	Initialise();

#ifdef _WIN32
	SetConsoleCursorPosition(s_Handle, COORD { static_cast<SHORT>(x), static_cast<SHORT>(y) });
#elif __unix__ || __APPLE__
	printf("\033[%d;%dH", y + 1, x + 1);
#endif
}

// Untested
void Console::GetCursorPos(int* x, int* y)
{
	Initialise();

#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO bufferInfo = GetScreenBufferInfo();
	*x = bufferInfo.dwCursorPosition.X;
	*y = bufferInfo.dwCursorPosition.Y;
#elif __unix__ || __APPLE__
	printf("\033[6n"); // Writes current cursor coordinates to terminal
	scanf("\033[%d;%dR", x, y); // Reads the cursor coordinates from terminal
#endif
}

void Console::Write(const std::string& text, ConsoleColourCombo colours)
{
#if _WIN32
	auto colour = static_cast<unsigned int>(colours.foreground);

	if(colours.background != ConsoleColour::Unchanged)
		colour = static_cast<unsigned>(colours.background) << 4 | colour;

	SetConsoleTextAttribute(s_Handle, static_cast<WORD>(colour));
	cout << text; // Write to standard output
	SetConsoleTextAttribute(s_Handle, s_Attributes); // Reset colours
#elif __unix__ || __APPLE__
	unsigned int fgCode = (unsigned int)colours.foreground;
	unsigned int bgCode = (unsigned int)colours.background + 10;

	if(colours.foreground == ConsoleColour::Unchanged) fgCode = 0;
	if(colours.background == ConsoleColour::Unchanged) bgCode = 0;

	printf("\033[%d;%dm%s\033[0m", bgCode, fgCode, text.c_str());
#endif
}
void Console::Write(const std::string& text, const ConsoleColour foreground, const ConsoleColour background) { Write(text, { foreground, background }); }

// Appends '\n' to `text` and writes
void Console::WriteLine(const std::string& text, const ConsoleColourCombo colours) { Write(text + "\n", colours); }

// Appends '\n' to `text` and writes
void Console::WriteLine(const std::string& text, const ConsoleColour foreground, const ConsoleColour background) { Write(text + "\n", foreground, background); }

// Proxy functions for char -> string
void Console::Write(const char& letter, const ConsoleColourCombo colours) { Write(string(1, letter), colours); }
void Console::Write(const char& letter, const ConsoleColour foreground, const ConsoleColour background) { Write(string(1, letter), { foreground, background }); }
void Console::WriteLine(const char& letter, const ConsoleColour foreground, const ConsoleColour background) { WriteLine(string(1, letter), { foreground, background }); }

void Console::SetTitle(const string& title)
{
	Initialise();

#ifdef _WIN32
	SetConsoleTitleA(title.c_str());
#elif __unix__ || __APPLE__
	printf("\033]0;%s", title.c_str()); // Don't think this works?
#endif
}