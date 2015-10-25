#ifndef LEIFTUR_PLATFORM_SPECIFIC
#define LEIFTUR_PLATFORM_SPECIFIC

#include <string>

namespace Leiftur
{
	namespace PlatformSpecific
	{
		std::string GetDllDirectory();
		void StartEditor(int port);
	}
}

#endif

