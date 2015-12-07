#ifndef POLYHEDRUS_WAVETABLE_MANAGER
#define POLYHEDRUS_WAVETABLE_MANAGER

#include "Default.h"
#include <vector>
#include <string>
#include <memory>

namespace Polyhedrus
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
		std::vector<WavetableFile> WavetableFiles;

	private:
		// How many partials to include in each wave
		int WavetablePartials[NumWavetablePartials] = { 512,389,291,218,163,122,91,68,51,38,28,21,16,12,9,6,5,3,2,1 };

		// How large the wave for each wave is
		int WavetableSize[NumWavetablePartials] = { 2048,2048,1024,1024,1024,512,512,512,256,256,128,128,128,128,128,128,128,128,128,128 };

		// The offset from zero from the base partial table to the top (integrates the WavetableSize array)
		int WavetableOffset[NumWavetablePartials];

		int TotalSize;

		// Maps each midi note to the correct partial wave (0...NumPartials)
		int WavetableIndex[128] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,2,2,2,2,2,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,6,6,7,7,7,7,7,8,8,8,8,8,9,9,9,9,9,10,10,10,10,10,11,11,11,11,11,12,12,12,12,12,13,13,13,13,13,14,14,14,14,14,15,15,15,15,15,16,16,16,16,16,17,17,17,17,17,18,18,18,18,18,18,18,18,18,18,18,19,19,19 };

	private:
		std::vector<std::weak_ptr<Wavetable>> loadedWavetables;
		std::shared_ptr<Wavetable> ConvertTable(float* wavetable, int tableSize, int numTables);
		void Normalize(std::shared_ptr<Wavetable> wavetable);
		std::vector<WavetableFile> ScanWavetableFiles(std::string pluginDirectory);

	public:

		//static std::vector<Wavetable*> Wavetables;
		void Setup(std::string waveformDirectory);
		std::shared_ptr<Wavetable> LoadWavetable(int wtNum);
		int GetId(std::string selector);
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
			this->wavetableData = new float[dataSize]();
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