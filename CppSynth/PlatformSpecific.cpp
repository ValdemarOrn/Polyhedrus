#include "PlatformSpecific.h"
#include <Windows.h>
#include <string>

namespace Leiftur
{
	namespace PlatformSpecific
	{
		HINSTANCE GetMyModuleHandle()
		{
			MEMORY_BASIC_INFORMATION mbi;
			VirtualQuery(GetMyModuleHandle, &mbi, sizeof(mbi));
			return (HINSTANCE)(mbi.AllocationBase);
		}

		std::string GetDllDirectory()
		{
			char dllName[512];
			HINSTANCE thismodule = GetMyModuleHandle();
			GetModuleFileName(thismodule, dllName, 512);
			
			std::string filename(dllName);
			std::string directory;
			const size_t last_slash_idx = filename.rfind('\\');
			if (std::string::npos != last_slash_idx)
			{
				directory = filename.substr(0, last_slash_idx);
			}

			return directory;
		}

		void StartEditor(int port)
		{

		}

		void AttachEditor(void* windowPointer)
		{

		}

		void DetachEditor()
		{

		}
	}
}
