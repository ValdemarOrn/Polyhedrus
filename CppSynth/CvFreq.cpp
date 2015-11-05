#define _USE_MATH_DEFINES
#include <cmath>

#include "CvFreq.h"

namespace Leiftur
{
	double CvFreq::GetCvFreqHz(double cv)
	{
		// Voltage is 1V/OCt, 0V = C0 = 16.3516Hz
		// 10.3V = Max = 20614.33hz
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
		for (int i = 0; i < 10300; i++)
		{
			double cv = i / 1000.0;
			auto cvFreq = GetCvFreqHz(cv);
			auto compensatedFreq = Warp(cvFreq, samplerate);
			CvToFreq[i] = cvFreq;
			CvToFreqWarped[i] = compensatedFreq;
		}
	}

}