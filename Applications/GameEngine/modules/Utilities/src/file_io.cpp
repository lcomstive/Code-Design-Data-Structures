#include <fstream>
#include <filesystem>
#include <GameUtilities/log.hpp>
#include <GameUtilities/file_io.hpp>

using namespace std;

vector<char> Utilities::ReadFileBinary(const string& path)
{
	// TODO: Check directory exists
	if (path.empty() || !FileExists(path))
		return vector<char>();

	ifstream file(path, ios::ate | ios::binary);

	if (!file.is_open())
		return vector<char>();

	size_t fileSize = (size_t)file.tellg();
	vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

const string Utilities::ReadFileString(const string& path)
{
	// TODO: Check directory exists
	if (path.empty() || !FileExists(path))
		return "";

	ifstream file(path, ios::ate);
	GAME_LOG_ASSERT(file.is_open(), "Failed to open shader file '" + path + "'");

	if (!file.is_open())
		return "";

	size_t fileSize = (size_t)file.tellg();
	string buffer(fileSize, '\0');

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

void Utilities::WriteFileString(const string& path, const string data)
{
	ofstream file(path);
	file << data;
	file.close();
}

void Utilities::WriteFileBinary(const string& path, vector<char> data)
{
	ofstream file(path, ios::binary);
	file << data.data();
	file.close();
}


bool Utilities::FileExists(string path) { return std::filesystem::exists(path); }
