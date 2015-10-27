#ifndef LEIFTUR_UTILS
#define LEIFTUR_UTILS

#include <vector>
#include <string>
#include <sstream>

namespace Leiftur
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
}

#endif
