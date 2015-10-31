#ifndef LEIFTUR_WAVETABLE
#define LEIFTUR_WAVETABLE

#include "Default.h"
#include <vector>
#include <string>

namespace Leiftur
{
	class Wavetable
	{
	public:
		const static int NumPartials = 20;

		// How many partials to include in each wave
		static int WavetablePartials[NumPartials];

		// How large the wave for each wave is
		static int WavetableSize[NumPartials];

		// The offset from zero from the base partial table to the top (integrates the WavetableSize array)
		static int WavetableOffset[NumPartials];

		static int TotalSize;

		// Maps each midi note to the correct partial wave (0...NumPartials)
		static int WavetableIndex[128];

		static std::vector<Wavetable*> Wavetables;
		static void Setup();

		int Count;
		int WavetableDataSize;
		float* WavetableData;
		
		inline float* GetTable(int tableIndex, int partialIndex)
		{
			if (partialIndex < 0)
				partialIndex = 0;
			else if (partialIndex >= NumPartials)
				partialIndex = NumPartials - 1;

			if (tableIndex < 0)
				tableIndex = 0;
			else if (tableIndex >= Count)
				tableIndex = Count - 1;

			int idx = tableIndex * TotalSize + WavetableOffset[partialIndex];
			return &WavetableData[idx];
		}
	};
}

#endif