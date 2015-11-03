#ifndef LEIFTUR_WAVETABLE_MANAGER
#define LEIFTUR_WAVETABLE_MANAGER

#include "Default.h"
#include <vector>
#include <string>
#include <memory>

namespace Leiftur
{
	class Wavetable;

	class WavetableManager
	{
	public:
		const static int NumPartials = 20;

		static std::vector<std::string> WavetableFiles;

		// How many partials to include in each wave
		static int WavetablePartials[NumPartials];

		// How large the wave for each wave is
		static int WavetableSize[NumPartials];

		// The offset from zero from the base partial table to the top (integrates the WavetableSize array)
		static int WavetableOffset[NumPartials];

		static int TotalSize;

		// Maps each midi note to the correct partial wave (0...NumPartials)
		static int WavetableIndex[128];

		//static std::vector<Wavetable*> Wavetables;
		static void Setup(std::string waveformDirectory);

		static std::shared_ptr<Wavetable> LoadWavetable(int wtNum);
	};

	class Wavetable
	{
	public:
		int Count;
		int WavetableDataSize;

	private:
		float* wavetableData;

	public:
		inline Wavetable(int dataSize)
		{
			this->wavetableData = new float[dataSize];
		}

		inline ~Wavetable()
		{
			delete wavetableData;
		}

		inline float* GetTable(int tableIndex, int partialIndex)
		{
			if (partialIndex < 0)
				partialIndex = 0;
			else if (partialIndex >= WavetableManager::NumPartials)
				partialIndex = WavetableManager::NumPartials - 1;

			if (tableIndex < 0)
				tableIndex = 0;
			else if (tableIndex >= Count)
				tableIndex = Count - 1;

			int idx = tableIndex * WavetableManager::TotalSize + WavetableManager::WavetableOffset[partialIndex];
			return &wavetableData[idx];
		}
	};
}

#endif