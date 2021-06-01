#include <ctime>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <GameUtilities/log.hpp>

using namespace std;
using namespace std::chrono;
using namespace Utilities;
using namespace std::filesystem;

const string& Log::LogFilePath = "./log.txt";
const string& Log::ErrorFilePath = "./error.txt";

mutex Log::m_LogMutex;
mutex Log::m_ErrorMutex;
atomic_bool Log::m_Initialized = false;

string currentTime()
{
	time_t timeNow = system_clock::to_time_t(system_clock::now());

	string s(6, '\0');
	strftime(&s[0], s.size(), "%H:%M", localtime(&timeNow));
	s = s.substr(0, 5); // Remove trailing '\0'

	auto epoch = system_clock::now().time_since_epoch();
	double timeMilliseconds = (double)(duration_cast<milliseconds>(epoch) - duration_cast<minutes>(epoch)).count();

	// Convert to seconds
	timeMilliseconds = ((int)timeMilliseconds) / 1000.0;

	s += ":" + to_string(timeMilliseconds).substr(0, 6); // seconds to 3 decimal places
	return s;
}

void Log::Initialize(bool outputToConsole)
{
	if (m_Initialized.load())
		return;

	ofstream logFilestream(LogFilePath);
	ofstream errorFilestream(ErrorFilePath);

	time_t timeNow = system_clock::to_time_t(system_clock::now());
	/// Date and time string in format "day/month/year hour:minute:second" "00/00/00 00:00:00"
	string dateString(18, '\0');
	strftime(dateString.data(), dateString.size(), "%d/%m/%y %H:%M:%S", localtime(&timeNow));
	dateString = dateString.substr(0, dateString.size() - 1); // Remove trailing '\0'

	logFilestream << "------------------------------------" << endl
				  << "---------- Mad Engine Log ----------" << endl
				  << "------------------------------------" << endl
				  << "Local time: " << dateString << endl << endl;
	errorFilestream << "------------------------------------" << endl
					<< "------- Mad Engine Error Log -------" << endl
					<< "------------------------------------" << endl
					<< "Local time: " << dateString << endl << endl;

	m_Initialized.store(true);

	MessageBus::eventBus()->AddReceiver("LogInfo", [](DataStream stream) { WriteToFile("[INFO]    " + stream.read<string>()); });
	MessageBus::eventBus()->AddReceiver("LogDebug", [](DataStream stream) { WriteToFile("[DEBUG]   " + stream.read<string>()); });
	MessageBus::eventBus()->AddReceiver("LogWarning", [](DataStream stream) { WriteToFile("[WARNING] " + stream.read<string>()); });
	MessageBus::eventBus()->AddReceiver("LogError", [](DataStream stream) { WriteToFile(stream.read<string>(), true); });

	if (outputToConsole)
	{
		MessageBus::eventBus()->AddReceiver("LogInfo", [](DataStream stream) { cout << "[INFO] " << stream.read<string>() << endl; });
		MessageBus::eventBus()->AddReceiver("LogDebug", [](DataStream stream) { cout << "[DEBUG] " << stream.read<string>() << endl; });
		MessageBus::eventBus()->AddReceiver("LogWarning", [](DataStream stream) { cout << "[WARNING] " << stream.read<string>() << endl; });
		MessageBus::eventBus()->AddReceiver("LogError", [](DataStream stream) { cerr << "[ERROR] " << stream.read<string>() << endl; });
	}
}

void Log::WriteToFile(string message, bool error)
{
	lock_guard<mutex> guard(error ? m_ErrorMutex : m_LogMutex);
	ofstream filestream = ofstream(error ? ErrorFilePath : LogFilePath, ios_base::app);
	filestream << "[" << currentTime() << "]";
	filestream << message << "\n";
	filestream.close();
}