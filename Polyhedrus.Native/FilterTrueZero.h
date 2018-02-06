#pragma once
#include <cmath>
#include "CvFreq.h"
#include "AudioLib/OnePoleFilters.h"
#include "FilterMain.h"

namespace Polyhedrus
{
	class FilterTrueZero : public FilterMain
	{
	private:
		static const int CVtoAlphaSize = 10500;
		static float CVtoAlpha[CVtoAlphaSize];
		static void ComputeCVtoAlpha(int samplerate);
	public:
		static inline float GetCvFreq(float cv)
		{
			// Voltage is 1V/OCt, C0 = 16.3516Hz
			// 10.3V = Max = 20614.33hz
			float freq = (float)(440.0 * std::pow(2, (cv * 12 - 69.0 + 12) / 12));
			return freq;
		}
		
	private:
		CvFreq CvToFreq;
		float* buffer;
		float gain;
		float gainInv;
		float totalResonance;

		AudioLib::Lp1 stage1;
		AudioLib::Lp1 stage2;
		AudioLib::Lp1 stage3;
		AudioLib::Lp1 stage4;
		float stage4Output;
		float convergenceRate;
		float outputDiff;
		int cntr;

		float fsinv;
		int samplerate;
		int bufferSize;
		int modulationUpdateRate;

	public:
		FilterTrueZero();
		~FilterTrueZero() override;

		FilterMainType GetType() const override;
		void Initialize(int samplerate, int bufferSize, int modulationUpdateRate) override;
		void SetParameter(FilterMainParameters parameter, double value) override;
		void Process(float* input, int len) override;
		inline float* GetOutput() override;
		
		std::vector<uint8_t> GetVisual() override;
		std::vector<uint8_t> GetDriveVisual() override;
		std::string GetModeString() override;

	private:
		void ProcessSample(float input);
		void Update();
	};
}

