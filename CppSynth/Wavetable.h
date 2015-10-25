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

		// These arrays map each midi note to a number of partials and the table index for that partial wave
		static int WavetablePartials[NumPartials];
		static int WavetableSize[NumPartials];
		static int WavetableIndex[128];

		static std::vector<Wavetable*> Wavetables;
		static void Setup();

		int Partials;
		int Count;
		int SampleSize;
		float* WavetableData;

		inline float* GetTable(int partial, int tableIndex)
		{
			return &WavetableData[SampleSize * (partial * Count + tableIndex)];
		}
	};
}

#endif