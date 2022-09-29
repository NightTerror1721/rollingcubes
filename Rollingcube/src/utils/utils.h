#pragma once

#include <string>
#include <algorithm>
#include <fstream>


namespace utils
{
	inline std::string lower(std::string_view str)
	{
		std::string result = std::string(str);
		std::transform(str.begin(), str.end(), result.begin(),
			[](char c) { return std::tolower(c); });
		return result;
	}

	inline std::string upper(std::string_view str)
	{
		std::string result = std::string(str);
		std::transform(str.begin(), str.end(), result.begin(),
			[](char c) { return std::toupper(c); });
		return result;
	}

	inline std::size_t file_size(std::ifstream& file)
	{
		file.seekg(0, std::ios::end);
		std::size_t size = file.tellg();
		file.seekg(0, std::ios::beg);

		return size;
	}
}
