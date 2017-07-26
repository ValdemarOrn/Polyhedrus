#ifndef POLYHEDRUS_FILTER_MAIN
#define POLYHEDRUS_FILTER_MAIN

#include "FilterCascade.h"
#include "FilterCascadeZero.h"
#include "FilterTrueZero.h"
#include "FilterDualSvf.h"
#include "Parameters.h"
#include "FilterInternalMode.h"
#include "AudioLib/OnePoleFilters.h"

namespace Polyhedrus
{
	class FilterMain
	{
	private:
		int samplerate;
		int modulationUpdateRate;
		FilterCascade cascadeFilter;
		FilterTrueZero trueZeroFilter;
		FilterCascadeZero cascadeZeroFilter;
		FilterDualSvf svfFilter;
		AudioLib::Hp1 hp;
		float* bypassBuffer;
		int type;

	public:
		bool IsEnabled;
		float cutoff;
		float resonance;
		float drive;

		FilterMain();
		~FilterMain();
		void Initialize(int samplerate, int bufferSize, int modulationUpdateRate);
		void SetParameter(FilterMainParameters parameter, double value);
		void Process(float* input, int len);
		float* GetOutput();
		std::vector<uint8_t> GetVisual();
		std::vector<uint8_t> GetDriveVisual();

		inline void SetCutoffMod(float modAmount)
		{
			cascadeFilter.CutoffMod = modAmount;
			cascadeZeroFilter.CutoffMod = modAmount;
			svfFilter.CutoffMod = modAmount;
			trueZeroFilter.CutoffMod = modAmount;
		}

		inline void SetDriveMod(float modAmount)
		{
			cascadeFilter.DriveMod = modAmount;
			cascadeZeroFilter.DriveMod = modAmount;
			svfFilter.DriveMod = modAmount;
			trueZeroFilter.DriveMod = modAmount;
		}

		inline void SetResonanceMod(float modAmount)
		{
			cascadeFilter.ResonanceMod = modAmount;
			cascadeZeroFilter.ResonanceMod = modAmount;
			svfFilter.ResonanceMod = modAmount;
			trueZeroFilter.ResonanceMod = modAmount;
		}
	};
}

#endif

