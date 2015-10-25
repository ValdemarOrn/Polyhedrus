#ifndef LEIFTUR_PLATFORM_SPECIFIC
#define LEIFTUR_PLATFORM_SPECIFIC

#include <string>

namespace Leiftur
{
	class PlatformSpecific
	{
	public:
		static std::string GetDllDir();
		static void StartEditor(int port);
	};
}

#endif

