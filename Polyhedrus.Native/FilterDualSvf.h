#ifndef POLYHEDRUS_FILTER_DUAL_SVF
#define POLYHEDRUS_FILTER_DUAL_SVF

#include <vector>
#include "FilterMain.h"
#include "AudioLib/SvfFilter.h"
#include "CvFreq.h"

namespace Polyhedrus
{
	class FilterDualSvf : public FilterMain
	{
	private:
		float* buffer;
		float gain;
		float gainInv;
		float totalResonance;
		float volLp;
		float volBp;
		float volHp;
		CvFreq cvToFreq;
		AudioLib::SvfFilter svf1;
		AudioLib::SvfFilter svf2;

		float fsinv;
		int samplerate;
		int modulationUpdateRate;

	public:
		FilterDualSvf();
		~FilterDualSvf() override;

		FilterMainType GetType() const override;
		void Initialize(int samplerate, int bufferSize, int modulationUpdateRate) override;
		void SetParameter(FilterMainParameters parameter, double value) override;
		void Process(float* input, int len) override;
		float* GetOutput() override;

		std::vector<uint8_t> GetVisual() override;
		std::vector<uint8_t> GetDriveVisual() override;
		std::string GetModeString() override;

		std::vector<float> GetMagnitudeResponse() const;

	private:
		void Update();
	};

}

#endif
