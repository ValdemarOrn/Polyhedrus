#ifndef LEIFTUR_WAVETABLE_MANAGER
#define LEIFTUR_WAVETABLE_MANAGER

#include "Default.h"
#include <vector>
#include <string>
#include <memory>

namespace Leiftur
{
	class Wavetable;

	class WavetableFile
	{
	public:
		std::string FilePath;
		std::string Selector;
		int Index;
	};

	const static int NumWavetablePartials = 20;

	class WavetableManager
	{
	public:
		static std::vector<WavetableFile> WavetableFiles;
	private:
		// How many partials to include in each wave
		static int WavetablePartials[NumWavetablePartials];

		// How large the wave for each wave is
		static int WavetableSize[NumWavetablePartials];

		// The offset from zero from the base partial table to the top (integrates the WavetableSize array)
		static int WavetableOffset[NumWavetablePartials];

		static int TotalSize;

		// Maps each midi note to the correct partial wave (0...NumPartials)
		static int WavetableIndex[128];

	private:
		static std::vector<std::weak_ptr<Wavetable>> loadedWavetables;
		static std::shared_ptr<Wavetable> ConvertTable(float* wavetable, int tableSize, int numTables);
		static void Normalize(std::shared_ptr<Wavetable> wavetable);
		static std::vector<WavetableFile> ScanWavetableFiles(std::string pluginDirectory);

	public:

		//static std::vector<Wavetable*> Wavetables;
		static void Setup(std::string waveformDirectory);

		static std::shared_ptr<Wavetable> LoadWavetable(int wtNum);
	};

	class Wavetable
	{
	public:
		int Count;
		int WavetableDataSize;

		// How many partials to include in each wave
		int WavetablePartials[NumWavetablePartials];
		// How large the wave for each wave is
		int WavetableSize[NumWavetablePartials];
		// The offset from zero from the base partial table to the top (integrates the WavetableSize array)
		int WavetableOffset[NumWavetablePartials];
		int TotalSize;
		// Maps each midi note to the correct partial wave (0...NumPartials)
		int WavetableIndex[128];

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
			else if (partialIndex >= NumWavetablePartials)
				partialIndex = NumWavetablePartials - 1;

			if (tableIndex < 0)
				tableIndex = 0;
			else if (tableIndex >= Count)
				tableIndex = Count - 1;

			int idx = tableIndex * TotalSize + WavetableOffset[partialIndex];
			return &wavetableData[idx];
		}
	};
}

#endif