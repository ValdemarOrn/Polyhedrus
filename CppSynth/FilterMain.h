#ifndef LEIFTUR_FILTER_MAIN
#define LEIFTUR_FILTER_MAIN

#include "FilterCascade.h"
#include "FilterCascadeZero.h"
#include "Parameters.h"

namespace Leiftur
{
	class FilterMain
	{
	private:
		int samplerate;
		int modulationUpdateRate;
		FilterCascade cascadeFilter;
		FilterCascadeZero cascadeZeroFilter;
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
		}

		inline void SetDriveMod(float modAmount)
		{
			cascadeFilter.DriveMod = modAmount;
			cascadeZeroFilter.DriveMod = modAmount;
		}

		inline void SetResonanceMod(float modAmount)
		{
			cascadeFilter.ResonanceMod = modAmount;
			cascadeZeroFilter.ResonanceMod = modAmount;
		}
	};
}

#endif

