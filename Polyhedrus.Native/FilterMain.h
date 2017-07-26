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
	enum class FilterMainType
	{
		TrueZero = 0,
		DualSvf = 1,
	};

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
		FilterMainType type;

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
		inline std::string GetModeString()
		{
			if (type == FilterMainType::DualSvf)
			{
				float mode = svfFilter.Mode * 2;
				if (mode <= 1)
				{
					return SPrint("%02.0f", (1 - mode) * 100) + "% LP - " + SPrint("%02.0f", mode * 100) + "% BP";
				}
				else
				{
					mode = mode - 1;
					return SPrint("%02.0f", (1 - mode) * 100) + "% BP - " + SPrint("%02.0f", mode * 100) + "% HP";
				}
			}
			else
			{
				return std::string("000");
			}
		}

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

