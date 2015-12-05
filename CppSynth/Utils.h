#ifndef POLYHEDRUS_UTILS
#define POLYHEDRUS_UTILS

#include <vector>
#include <string>
#include <sstream>

namespace Polyhedrus
{
	inline std::vector<std::string> SplitString(const std::string s, char delim)
	{
		std::vector<std::string> elems;

		std::stringstream ss(s);
		std::string item;

		while (std::getline(ss, item, delim))
		{
			elems.push_back(item);
		}
		return elems;
	}

	inline std::string SPrint(const char* formatter, double value)
	{
		char buffer[256];
		sprintf(buffer, formatter, value);
		return std::string(buffer);
	}
}

#endif
