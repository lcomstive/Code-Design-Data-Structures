#pragma once
#include <vector>
#include <string>

namespace Utilities
{
	const std::string ReadFileString(const std::string& path);
	std::vector<char> ReadFileBinary(const std::string& path);

	void WriteFileString(const std::string& path, const std::string data);
	void WriteFileBinary(const std::string& path, std::vector<char> data);

	bool FileExists(std::string path);
}