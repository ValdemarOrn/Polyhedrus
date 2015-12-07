#ifndef POLYHEDRUS_PLATFORM_SPECIFIC
#define POLYHEDRUS_PLATFORM_SPECIFIC

#include <string>

namespace Polyhedrus
{
	class PlatformSpecific
	{
	public:
		static std::string GetDllDir();
		static void StartEditor(int port);

		static long long PerformanceFrequency();
		static long long PerformanceCounter();
	};
}

#endif

