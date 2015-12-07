#include "AudioLib/MathDefs.h"
#include <cmath>

#include "CvFreq.h"

namespace Polyhedrus
{
	double CvFreq::GetCvFreqHz(double cv)
	{
		// Voltage is 1V/OCt, 0V = C0 = 16.3516Hz
		// 10.3V = Max = 20614.33hz
		// goes up to 11V = 33488.07 Hz as some filters need it
		double freq = (double)(440.0 * std::pow(2, (cv * 12 - 69.0 + 12) / 12));
		return freq;
	}

	double CvFreq::Warp(double freqHz, double samplerate)
	{
		double T = 1.0 / samplerate;
		double omegaCWarped = freqHz * 2 * M_PI;
		double omegaC = 2 / T * std::tan(omegaCWarped * T / 2);

		return omegaC / 2 / M_PI;
	}

	void CvFreq::Initialize(float samplerate)
	{
		for (int i = 0; i < 11000; i++)
		{
			double cv = i / 1000.0;
			auto cvFreq = GetCvFreqHz(cv);
			auto compensatedFreq = Warp(cvFreq, samplerate);
			CvToFreq[i] = (float)cvFreq;
			CvToFreqWarped[i] = (float)compensatedFreq;
		}
	}

}