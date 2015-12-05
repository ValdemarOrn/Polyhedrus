#ifndef POLYHEDRUS_FILTER_MAIN
#define POLYHEDRUS_FILTER_MAIN

#include "FilterCascade.h"
#include "FilterCascadeZero.h"
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
		FilterCascadeZero cascadeZeroFilter;
		FilterDualSvf svfFilter;
		AudioLib::Hp1 hp;
		float* bypassBuffer;
		int type;

	public:
		bool IsEnabled;
		FilterMain();
		~FilterMain();
		void Initialize(int samplerate, int bufferSize, int modulationUpdateRate);
		void SetParameter(FilterMainParameters parameter, double value);
		void Process(float* input, int len);
		float* GetOutput();

		inline void SetCutoffMod(float modAmount)
		{
			cascadeFilter.CutoffMod = modAmount;
			cascadeZeroFilter.CutoffMod = modAmount;
			svfFilter.CutoffMod = modAmount;
		}

		inline void SetDriveMod(float modAmount)
		{
			cascadeFilter.DriveMod = modAmount;
			cascadeZeroFilter.DriveMod = modAmount;
			svfFilter.DriveMod = modAmount;
		}

		inline void SetResonanceMod(float modAmount)
		{
			cascadeFilter.ResonanceMod = modAmount;
			cascadeZeroFilter.ResonanceMod = modAmount;
			svfFilter.ResonanceMod = modAmount;
		}
	};
}

#endif

